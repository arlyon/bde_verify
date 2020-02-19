// csabbg_deprecated.cpp                                              -*-C++-*-

#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/DeclarationName.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/Specifiers.h>
#include <csabase_analyser.h>
#include <csabase_config.h>
#include <csabase_debug.h>
#include <csabase_diagnostic_builder.h>
#include <csabase_location.h>
#include <csabase_ppobserver.h>
#include <csabase_registercheck.h>
#include <csabase_report.h>
#include <csabase_util.h>
#include <csabase_visitor.h>
#include <ctype.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>
#include <stddef.h>
#include <stdlib.h>
#include <utils/event.hpp>
#include <utils/function.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace clang;
using namespace csabase;

// ----------------------------------------------------------------------------

static std::string const check_name("deprecated");

// ----------------------------------------------------------------------------

namespace
{

struct data
    // Data attached to analyzer for this check.
{
    typedef std::vector<SourceRange> Ranges;
    typedef std::map<std::string, Ranges> Comments;
    Comments d_comments;  // Comment blocks per file.

    typedef std::map<std::string, SourceLocation> DeprecatedFiles;
    DeprecatedFiles d_dep_files;

    typedef std::map<SourceLocation, SourceLocation> DeprecatedComments;
    DeprecatedComments d_dep_comms;

    typedef std::map<const FunctionDecl*, SourceLocation> DeprecatedFunctions;
    DeprecatedFunctions d_dep_funcs;

    typedef std::map<const CXXRecordDecl*, SourceLocation> DeprecatedRecords;
    DeprecatedRecords d_dep_recs;

    typedef std::vector<std::pair<const FunctionDecl*, SourceRange>> FunDecls;
    FunDecls d_fundecls;  // FunctionDecl, comment

    typedef std::vector<std::pair<const CXXRecordDecl*, SourceRange>> RecDecls;
    RecDecls d_recdecls;  // CXXRecordDecl, comment

    typedef std::multimap<Location, const FunctionDecl*> Calls;
    Calls d_calls;
};

struct report : Report<data>
    // Callback object invoked upon completion.
{
    INHERIT_REPORT_CTOR(report, Report, data);

    void operator()();
        // Invoked to process reports.

    void operator()(const CXXRecordDecl *rec);
    void operator()(const ClassTemplateDecl *rec);
    void operator()(const FunctionDecl *func);
    void operator()(const FunctionTemplateDecl *func);
    void operator()(const CallExpr *call);

    void operator()(SourceRange range);
        // The specified 'range', representing a comment, is either appended to
        // the previous comment or added separately to the comments list.

    SourceRange getContract(const FunctionDecl     *func,
                            data::Ranges::iterator  comments_begin,
                            data::Ranges::iterator  comments_end);
        // Return the 'SourceRange' of the function contract of the specified
        // 'func' if it is present in the specified range of 'comments_begin'
        // up to 'comments_end', and return an invalid 'SourceRange' otherwise.

    SourceRange getContract(const CXXRecordDecl    *rec,
                            data::Ranges::iterator  comments_begin,
                            data::Ranges::iterator  comments_end);
        // Return the 'SourceRange' of the class contract of the specified
        // 'rec' if it is present in the specified range of 'comments_begin' up
        // to 'comments_end', and return an invalid 'SourceRange' otherwise.
};

void report::operator()(SourceRange range)
{
    Location location(a.get_location(range.getBegin()));
    data::Ranges& c = d.d_comments[location.file()];
    if (c.size() == 0 || !areConsecutive(m, c.back(), range)) {
        c.push_back(range);
    } else {
        c.back().setEnd(range.getEnd());
    }
    if (!d.d_dep_files.count(location.file()) &&
        a.get_source(range).startswith("//@DEPRECATED:")) {
        d.d_dep_files[location.file()] = range.getBegin();
    }
    if (!d.d_dep_comms.count(c.back().getBegin()) &&
        a.get_source(range).startswith("// !DEPRECATED!:")) {
        d.d_dep_comms[c.back().getBegin()] = range.getBegin();
    }
}

void report::operator()(const CallExpr *call)
{
    if (auto func = call->getDirectCallee()) {
        if (auto f = func->getTemplateInstantiationPattern()) {
            func = f;
        }
        func = func->getCanonicalDecl();
        if (!a.is_test_driver() ||
            !a.is_component_header(func->getBeginLoc())) {
            d.d_calls.insert(
                std::make_pair(Location(m, call->getExprLoc()), func));
        }
    }
}

void report::operator()()
{
    for (auto& p : d.d_recdecls) {
        Location location(a.get_location(p.first->getBeginLoc()));
        data::Ranges& c = d.d_comments[location.file()];
        p.second = getContract(p.first, c.begin(), c.end());
        auto j = d.d_dep_comms.find(p.second.getBegin());
        if (j != d.d_dep_comms.end()) {
            d.d_dep_recs[p.first] = j->second;
        }
    }

    for (auto& p : d.d_fundecls) {
        Location location(a.get_location(p.first->getBeginLoc()));
        data::Ranges& c = d.d_comments[location.file()];
        p.second = getContract(p.first, c.begin(), c.end());
        auto i = d.d_dep_files.find(location.file());
        if (i != d.d_dep_files.end()) {
            d.d_dep_funcs[p.first] = i->second;
        }
        if (auto h = llvm::dyn_cast<CXXMethodDecl>(p.first)) {
            auto k = d.d_dep_recs.find(h->getParent()->getDefinition());
            if (k != d.d_dep_recs.end()) {
                d.d_dep_funcs[p.first] = k->second;
            }
        }
        auto j = d.d_dep_comms.find(p.second.getBegin());
        if (j != d.d_dep_comms.end()) {
            d.d_dep_funcs[p.first] = j->second;
        }
    }

    for (auto& p : d.d_calls) {
        auto i = d.d_dep_funcs.find(p.second);
        if (i != d.d_dep_funcs.end() && !a.is_component(i->second)) {
            a.report(p.first.location(), check_name, "DP01",
                     "Call to deprecated function");
            a.report(i->second, check_name, "DP01",
                     "Deprecated here",
                     false, DiagnosticIDs::Note);
        }
    }
}

void report::operator()(const FunctionTemplateDecl* func)
{
    (*this)(func->getTemplatedDecl());
}

void report::operator()(const FunctionDecl* func)
{
    // Don't process compiler-defaulted methods, main, template instantiations,
    // or macro expansions
    if (   !func->isDefaulted()
        && !func->isMain()
        && !func->getLocation().isMacroID()
        && (   func->getTemplatedKind() == func->TK_NonTemplate
            || func->getTemplatedKind() == func->TK_FunctionTemplate)
            ) {
        if (auto f = func->getTemplateInstantiationPattern()) {
            func = f;
        }
        d.d_fundecls.push_back(
            std::make_pair(func->getCanonicalDecl(), SourceRange()));
    }
}

void report::operator()(const ClassTemplateDecl* rec)
{
    (*this)(rec->getTemplatedDecl());
}

void report::operator()(const CXXRecordDecl* rec)
{
    // Don't process template instantiations or macro expansions
    if (   !rec->getLocation().isMacroID()
        && !rec->getTemplateInstantiationPattern()
        && rec->hasDefinition()) {
        d.d_recdecls.push_back(
            std::make_pair(rec->getDefinition(), SourceRange()));
    }
}

SourceRange report::getContract(const FunctionDecl     *func,
                                data::Ranges::iterator  comments_begin,
                                data::Ranges::iterator  comments_end)
{
    SourceRange declarator = func->getSourceRange();
    declarator.setEnd(declarator.getEnd().getLocWithOffset(1));
    SourceRange contract;
    bool with_body = func->doesThisDeclarationHaveABody() && func->getBody();
    bool one_liner = with_body &&
                     m.getPresumedLineNumber(declarator.getBegin()) ==
                         m.getPresumedLineNumber(func->getBody()->getEndLoc());

    const CXXConstructorDecl *ctor = llvm::dyn_cast<CXXConstructorDecl>(func);

    if (ctor && with_body && ctor->getNumCtorInitializers() > 0) {
        // Constructor with body and initializers - look for a contract that
        // starts no earlier than the first initializer and has only whitespace
        // and a colon between itself and that initializer.
        SourceLocation initloc = (*ctor->init_begin())->getSourceLocation();
        if (initloc.isValid()) {
            data::Ranges::iterator it;
            for (it = comments_begin; it != comments_end; ++it) {
                if (m.isBeforeInTranslationUnit(initloc, it->getBegin())) {
                    break;
                }
                if (m.isBeforeInTranslationUnit(
                        it->getEnd(), declarator.getBegin())) {
                    continue;
                }
                llvm::StringRef s =
                    a.get_source(SourceRange(it->getEnd(), initloc), true);
                if (s.find_first_not_of(": \r\n") == llvm::StringRef::npos) {
                    contract = *it;
                    break;
                }
            }
        }
    }

    if (with_body && !contract.isValid()) {
        // Function with body - look for a comment that starts no earlier than
        // the function declarator and has only whitespace between itself and
        // the open brace of the function.
        SourceLocation bodyloc = func->getBody()->getBeginLoc();
        data::Ranges::iterator it;
        for (it = comments_begin; it != comments_end; ++it) {
            if (m.isBeforeInTranslationUnit(bodyloc, it->getBegin())) {
                break;
            }
            if (m.isBeforeInTranslationUnit(
                    it->getEnd(), declarator.getBegin())) {
                continue;
            }
            llvm::StringRef s =
                a.get_source(SourceRange(it->getEnd(), bodyloc), true);
            if (s.find_first_not_of(" \r\n") == llvm::StringRef::npos) {
                contract = *it;
                break;
            }
        }
    }

    if (!with_body || (one_liner && !contract.isValid())) {
        // Function without body or one-liner - look for a comment following
        // the declaration separated from it by only whitespace and semicolon.
        SourceLocation endloc = declarator.getEnd();
        data::Ranges::iterator it;
        for (it = comments_begin; it != comments_end; ++it) {
            if (m.isBeforeInTranslationUnit(it->getEnd(), endloc)) {
                continue;
            }
            llvm::StringRef s =
                a.get_source(SourceRange(endloc, it->getBegin()), true);
            if (!with_body) {
                s = s.split(';').second;
            }
            if (s.find_first_not_of(" \r\n") == llvm::StringRef::npos &&
                s.count("\n") <= 1) {
                contract = *it;
            }
            break;
        }
    }
    return contract;
}

SourceRange report::getContract(const CXXRecordDecl    *rec,
                                data::Ranges::iterator  comments_begin,
                                data::Ranges::iterator  comments_end)
{
    SourceRange contract;

    if (rec->hasDefinition()) {
        rec = rec->getDefinition();
        SourceRange range = rec->getSourceRange();
        auto b = rec->decls_begin();
        for (; b != rec->decls_end(); ++b) {
            if (b->getBeginLoc() != range.getBegin()) {
                break;
            }
        }
        if (b != rec->decls_end()) {
            range.setEnd(b->getBeginLoc());
        }

        data::Ranges::iterator it;
        for (it = comments_begin; it != comments_end; ++it) {
            if (m.isBeforeInTranslationUnit(range.getEnd(), it->getBegin())) {
                break;
            }
            if (m.isBeforeInTranslationUnit(it->getEnd(), range.getBegin())) {
               continue;
            }
            contract = *it;
            break;
        }
    }

    return contract;
}

void subscribe(Analyser& analyser, Visitor& visitor, PPObserver& observer)
    // Hook up the callback functions.
{
    analyser.onTranslationUnitDone += report(analyser);
    visitor.onFunctionDecl += report(analyser);
    visitor.onFunctionTemplateDecl += report(analyser);
    visitor.onCXXRecordDecl += report(analyser);
    visitor.onClassTemplateDecl += report(analyser);
    visitor.onCallExpr += report(analyser);
    observer.onComment += report(analyser);
}

}  // close anonymous namespace

// ----------------------------------------------------------------------------

static RegisterCheck c1(check_name, &subscribe);

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
