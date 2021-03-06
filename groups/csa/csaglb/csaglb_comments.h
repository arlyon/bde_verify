// csaglb_comments.h                                                  -*-C++-*-
#ifndef INCLUDED_CSAGLB_COMMENTS
#define INCLUDED_CSAGLB_COMMENTS

#include <clang/Basic/SourceLocation.h>
#include <map>
#include <string>
#include <vector>

namespace csabase { class Analyser; }

namespace csabase {
struct CommentData
    // Data holding seen comments.
{
    typedef std::vector<clang::SourceRange> Ranges;
    typedef std::map<std::string, Ranges>   CommentsByFile;
    CommentsByFile                          d_comments;
    Ranges                                  d_allComments;

    void append(Analyser& analyser, clang::SourceRange range);
};
}

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2017 Bloomberg Finance L.P.
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
