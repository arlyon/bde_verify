#ifndef INCLUDED_CSABASE_DEBUG_RAV
#define INCLUDED_CSABASE_DEBUG_RAV
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include <csabase_location.h>

#include <string>

template <typename Derived>
struct DebugRecursiveASTVisitor : clang::RecursiveASTVisitor<Derived>
{
    typedef clang::RecursiveASTVisitor<Derived> base;
    using base::getDerived;

    int d_level;

    DebugRecursiveASTVisitor() : d_level(0) { }

    struct Guard {
        DebugRecursiveASTVisitor *r;
        llvm::StringRef n;

        template <class V>
        Guard(DebugRecursiveASTVisitor *r, llvm::StringRef n, V v) : r(r), n(n)
        { llvm::errs().indent(r->d_level++) << "\\" << n; x(v) << "\n"; }
        ~Guard()
        { llvm::errs().indent(--r->d_level) << "/" << n << "\n"; }

        llvm::raw_ostream& z(clang::SourceRange u) {
            auto &a = r->getDerived().a;
            auto &m = r->getDerived().m;
            csabase::Range r(m, u);
            llvm::StringRef s = a.get_source(u);
            if (!s.size()) return llvm::errs();
            return llvm::errs()
                << " "
                << "@" << r.from().line() << ":" << r.from().column() << "@"
                << b(s)
                << "$" << r.to().line()   << ":" << r.to().column()   << "$";
        }

        llvm::raw_ostream& x(...) { return llvm::errs(); }
        llvm::raw_ostream& x(clang::QualType q)
        { return llvm::errs() << " <<<" << b(q.getAsString()) << ">>>"; }
        llvm::raw_ostream &x(clang::Stmt *e)
        { return e ? z(e->getSourceRange()) : llvm::errs(); }
        llvm::raw_ostream &x(clang::Decl *d)
        { return d ? z(d->getSourceRange()) : llvm::errs(); }
        llvm::raw_ostream &x(clang::Type *t)
        { return t ? x(QualType(t, 0)) : llvm::errs(); }
        llvm::raw_ostream& x(clang::TypeLoc tl) {
            z(tl.getSourceRange()) << " ";
            return z(tl.getLocalSourceRange());
        }
        llvm::raw_ostream& x(clang::NestedNameSpecifierLoc nl) {
            if (nl) {
                z(nl.getSourceRange()) << " ";
                z(nl.getLocalSourceRange());
            }
            return llvm::errs();
        }
        llvm::raw_ostream& x(clang::DeclarationNameInfo dni)
        { return z(dni.getSourceRange()); }

        static std::string b(llvm::StringRef s)
        {
            std::string ret;
            while (s.size()) {
                switch (s.front()) {
                case ' ': case '\n': case '\r': case '\t':
                    ret += ' ';
                    s = s.ltrim();
                    break;
                default:
                    ret += s.front();
                    s = s.drop_front();
                    break;
                }
            }
            return ret;
        }
    };

#undef  F
#define F(N,T) bool N(T arg) { Guard g(this, #N, arg); return base::N(arg); }

#undef  FT
#define FT(N,T) F(Traverse##N,T)
#undef  FV
#define FV(N,T) F(Visit##N,T)
#undef  FW
#define FW(N,T) F(WalkUpFrom##N,T)

#undef  PT
#define PT(N) FT(N,N*)
#undef  PV
#define PV(N) FV(N,N*)
#undef  PW
#define PW(N) FW(N,N*)
#undef  LT
#define LT(N) FT(N,N)
#undef  LV
#define LV(N) FV(N,N)
#undef  LW
#define LW(N) FW(N,N)

#undef  F2
#define F2(N,T1,T2)                                                           \
    bool N(T1 v1, T2 v2) { Guard g(this, #N, v1); return base::N(v1, v2); }
#undef  F2T
#define F2T(N,T1,T2) F2(Traverse##N, T1, T2)

#if DEBUG_RAV

bool TraverseDeclaratorHelper(clang::DeclaratorDecl *arg)
{
    Guard g(this, "TraverseDeclaratorHelper", arg);
    auto &d = getDerived();
    return d.TraverseNestedNameSpecifierLoc(arg->getQualifierLoc()) &&
           (arg->getTypeSourceInfo()
            ? d.TraverseTypeLoc(arg->getTypeSourceInfo()->getTypeLoc())
            : d.TraverseType(arg->getType()));
}

F2T(LambdaCapture, LambdaExpr *, const LambdaCapture *)
F2T(TemplateArguments, const TemplateArgument *, unsigned)
FT(BinAdd, BinaryOperator *)
FT(BinAddAssign, CompoundAssignOperator *)
FT(BinAnd, BinaryOperator *)
FT(BinAndAssign, CompoundAssignOperator *)
FT(BinAssign, BinaryOperator *)
FT(BinComma, BinaryOperator *)
FT(BinDiv, BinaryOperator *)
FT(BinDivAssign, CompoundAssignOperator *)
FT(BinEQ, BinaryOperator *)
FT(BinGE, BinaryOperator *)
FT(BinGT, BinaryOperator *)
FT(BinLAnd, BinaryOperator *)
FT(BinLE, BinaryOperator *)
FT(BinLOr, BinaryOperator *)
FT(BinLT, BinaryOperator *)
FT(BinMul, BinaryOperator *)
FT(BinMulAssign, CompoundAssignOperator *)
FT(BinNE, BinaryOperator *)
FT(BinOr, BinaryOperator *)
FT(BinOrAssign, CompoundAssignOperator *)
FT(BinPtrMemD, BinaryOperator *)
FT(BinPtrMemI, BinaryOperator *)
FT(BinRem, BinaryOperator *)
FT(BinRemAssign, CompoundAssignOperator *)
FT(BinShl, BinaryOperator *)
FT(BinShlAssign, CompoundAssignOperator *)
FT(BinShr, BinaryOperator *)
FT(BinShrAssign, CompoundAssignOperator *)
FT(BinSub, BinaryOperator *)
FT(BinSubAssign, CompoundAssignOperator *)
FT(BinXor, BinaryOperator *)
FT(BinXorAssign, CompoundAssignOperator *)
FT(ConstructorInitializer, CXXCtorInitializer *)
FT(DeclarationNameInfo, DeclarationNameInfo)
FT(LambdaBody, LambdaExpr *)
FT(NestedNameSpecifierLoc, NestedNameSpecifierLoc)
FT(TemplateArgument, const TemplateArgument &)
FT(TemplateArgumentLoc, const TemplateArgumentLoc &)
FT(TemplateName, TemplateName)
FT(Type, QualType)
FT(UnaryAddrOf, UnaryOperator *)
FT(UnaryDeref, UnaryOperator *)
FT(UnaryExtension, UnaryOperator *)
FT(UnaryImag, UnaryOperator *)
FT(UnaryLNot, UnaryOperator *)
FT(UnaryMinus, UnaryOperator *)
FT(UnaryNot, UnaryOperator *)
FT(UnaryPlus, UnaryOperator *)
FT(UnaryPostDec, UnaryOperator *)
FT(UnaryPostInc, UnaryOperator *)
FT(UnaryPreDec, UnaryOperator *)
FT(UnaryPreInc, UnaryOperator *)
FT(UnaryReal, UnaryOperator *)
FV(BinAdd, BinaryOperator *)
FV(BinAddAssign, CompoundAssignOperator *)
FV(BinAnd, BinaryOperator *)
FV(BinAndAssign, CompoundAssignOperator *)
FV(BinAssign, BinaryOperator *)
FV(BinComma, BinaryOperator *)
FV(BinDiv, BinaryOperator *)
FV(BinDivAssign, CompoundAssignOperator *)
FV(BinEQ, BinaryOperator *)
FV(BinGE, BinaryOperator *)
FV(BinGT, BinaryOperator *)
FV(BinLAnd, BinaryOperator *)
FV(BinLE, BinaryOperator *)
FV(BinLOr, BinaryOperator *)
FV(BinLT, BinaryOperator *)
FV(BinMul, BinaryOperator *)
FV(BinMulAssign, CompoundAssignOperator *)
FV(BinNE, BinaryOperator *)
FV(BinOr, BinaryOperator *)
FV(BinOrAssign, CompoundAssignOperator *)
FV(BinPtrMemD, BinaryOperator *)
FV(BinPtrMemI, BinaryOperator *)
FV(BinRem, BinaryOperator *)
FV(BinRemAssign, CompoundAssignOperator *)
FV(BinShl, BinaryOperator *)
FV(BinShlAssign, CompoundAssignOperator *)
FV(BinShr, BinaryOperator *)
FV(BinShrAssign, CompoundAssignOperator *)
FV(BinSub, BinaryOperator *)
FV(BinSubAssign, CompoundAssignOperator *)
FV(BinXor, BinaryOperator *)
FV(BinXorAssign, CompoundAssignOperator *)
FV(UnaryAddrOf, UnaryOperator *)
FV(UnaryDeref, UnaryOperator *)
FV(UnaryExtension, UnaryOperator *)
FV(UnaryImag, UnaryOperator *)
FV(UnaryLNot, UnaryOperator *)
FV(UnaryMinus, UnaryOperator *)
FV(UnaryNot, UnaryOperator *)
FV(UnaryPlus, UnaryOperator *)
FV(UnaryPostDec, UnaryOperator *)
FV(UnaryPostInc, UnaryOperator *)
FV(UnaryPreDec, UnaryOperator *)
FV(UnaryPreInc, UnaryOperator *)
FV(UnaryReal, UnaryOperator *)
FW(BinAdd, BinaryOperator *)
FW(BinAddAssign, CompoundAssignOperator *)
FW(BinAnd, BinaryOperator *)
FW(BinAndAssign, CompoundAssignOperator *)
FW(BinAssign, BinaryOperator *)
FW(BinComma, BinaryOperator *)
FW(BinDiv, BinaryOperator *)
FW(BinDivAssign, CompoundAssignOperator *)
FW(BinEQ, BinaryOperator *)
FW(BinGE, BinaryOperator *)
FW(BinGT, BinaryOperator *)
FW(BinLAnd, BinaryOperator *)
FW(BinLE, BinaryOperator *)
FW(BinLOr, BinaryOperator *)
FW(BinLT, BinaryOperator *)
FW(BinMul, BinaryOperator *)
FW(BinMulAssign, CompoundAssignOperator *)
FW(BinNE, BinaryOperator *)
FW(BinOr, BinaryOperator *)
FW(BinOrAssign, CompoundAssignOperator *)
FW(BinPtrMemD, BinaryOperator *)
FW(BinPtrMemI, BinaryOperator *)
FW(BinRem, BinaryOperator *)
FW(BinRemAssign, CompoundAssignOperator *)
FW(BinShl, BinaryOperator *)
FW(BinShlAssign, CompoundAssignOperator *)
FW(BinShr, BinaryOperator *)
FW(BinShrAssign, CompoundAssignOperator *)
FW(BinSub, BinaryOperator *)
FW(BinSubAssign, CompoundAssignOperator *)
FW(BinXor, BinaryOperator *)
FW(BinXorAssign, CompoundAssignOperator *)
FW(UnaryAddrOf, UnaryOperator *)
FW(UnaryDeref, UnaryOperator *)
FW(UnaryExtension, UnaryOperator *)
FW(UnaryImag, UnaryOperator *)
FW(UnaryLNot, UnaryOperator *)
FW(UnaryMinus, UnaryOperator *)
FW(UnaryNot, UnaryOperator *)
FW(UnaryPlus, UnaryOperator *)
FW(UnaryPostDec, UnaryOperator *)
FW(UnaryPostInc, UnaryOperator *)
FW(UnaryPreDec, UnaryOperator *)
FW(UnaryPreInc, UnaryOperator *)
FW(UnaryReal, UnaryOperator *)
LT(AdjustedTypeLoc)
LT(AtomicTypeLoc)
LT(AttributedTypeLoc)
LT(AutoTypeLoc)
LT(BlockPointerTypeLoc)
LT(BuiltinTypeLoc)
LT(ComplexTypeLoc)
LT(ConstantArrayTypeLoc)
LT(DecayedTypeLoc)
LT(DecltypeTypeLoc)
LT(DependentNameTypeLoc)
LT(DependentSizedArrayTypeLoc)
LT(DependentSizedExtVectorTypeLoc)
LT(DependentTemplateSpecializationTypeLoc)
LT(ElaboratedTypeLoc)
LT(EnumTypeLoc)
LT(ExtVectorTypeLoc)
LT(FunctionNoProtoTypeLoc)
LT(FunctionProtoTypeLoc)
LT(IncompleteArrayTypeLoc)
LT(InjectedClassNameTypeLoc)
LT(LValueReferenceTypeLoc)
LT(MemberPointerTypeLoc)
LT(ObjCInterfaceTypeLoc)
LT(ObjCObjectPointerTypeLoc)
LT(ObjCObjectTypeLoc)
LT(PackExpansionTypeLoc)
LT(ParenTypeLoc)
LT(PointerTypeLoc)
LT(QualifiedTypeLoc)
LT(RValueReferenceTypeLoc)
LT(RecordTypeLoc)
LT(SubstTemplateTypeParmPackTypeLoc)
LT(SubstTemplateTypeParmTypeLoc)
LT(TemplateSpecializationTypeLoc)
LT(TemplateTypeParmTypeLoc)
LT(TypeLoc)
LT(TypeOfExprTypeLoc)
LT(TypeOfTypeLoc)
LT(TypedefTypeLoc)
LT(UnaryTransformTypeLoc)
LT(UnresolvedUsingTypeLoc)
LT(VariableArrayTypeLoc)
LT(VectorTypeLoc)
LV(AdjustedTypeLoc)
LV(ArrayTypeLoc)
LV(AtomicTypeLoc)
LV(AttributedTypeLoc)
LV(AutoTypeLoc)
LV(BlockPointerTypeLoc)
LV(BuiltinTypeLoc)
LV(ComplexTypeLoc)
LV(ConstantArrayTypeLoc)
LV(DecayedTypeLoc)
LV(DecltypeTypeLoc)
LV(DependentNameTypeLoc)
LV(DependentSizedArrayTypeLoc)
LV(DependentSizedExtVectorTypeLoc)
LV(DependentTemplateSpecializationTypeLoc)
LV(ElaboratedTypeLoc)
LV(EnumTypeLoc)
LV(ExtVectorTypeLoc)
LV(FunctionNoProtoTypeLoc)
LV(FunctionProtoTypeLoc)
LV(FunctionTypeLoc)
LV(IncompleteArrayTypeLoc)
LV(InjectedClassNameTypeLoc)
LV(LValueReferenceTypeLoc)
LV(MemberPointerTypeLoc)
LV(ObjCInterfaceTypeLoc)
LV(ObjCObjectPointerTypeLoc)
LV(ObjCObjectTypeLoc)
LV(PackExpansionTypeLoc)
LV(ParenTypeLoc)
LV(PointerTypeLoc)
LV(QualifiedTypeLoc)
LV(RValueReferenceTypeLoc)
LV(RecordTypeLoc)
LV(ReferenceTypeLoc)
LV(SubstTemplateTypeParmPackTypeLoc)
LV(SubstTemplateTypeParmTypeLoc)
LV(TagTypeLoc)
LV(TemplateSpecializationTypeLoc)
LV(TemplateTypeParmTypeLoc)
LV(TypeLoc)
LV(TypeOfExprTypeLoc)
LV(TypeOfTypeLoc)
LV(TypedefTypeLoc)
LV(UnaryTransformTypeLoc)
LV(UnqualTypeLoc)
LV(UnresolvedUsingTypeLoc)
LV(VariableArrayTypeLoc)
LV(VectorTypeLoc)
LW(AdjustedTypeLoc)
LW(ArrayTypeLoc)
LW(AtomicTypeLoc)
LW(AttributedTypeLoc)
LW(AutoTypeLoc)
LW(BlockPointerTypeLoc)
LW(BuiltinTypeLoc)
LW(ComplexTypeLoc)
LW(ConstantArrayTypeLoc)
LW(DecayedTypeLoc)
LW(DecltypeTypeLoc)
LW(DependentNameTypeLoc)
LW(DependentSizedArrayTypeLoc)
LW(DependentSizedExtVectorTypeLoc)
LW(DependentTemplateSpecializationTypeLoc)
LW(ElaboratedTypeLoc)
LW(EnumTypeLoc)
LW(ExtVectorTypeLoc)
LW(FunctionNoProtoTypeLoc)
LW(FunctionProtoTypeLoc)
LW(FunctionTypeLoc)
LW(IncompleteArrayTypeLoc)
LW(InjectedClassNameTypeLoc)
LW(LValueReferenceTypeLoc)
LW(MemberPointerTypeLoc)
LW(ObjCInterfaceTypeLoc)
LW(ObjCObjectPointerTypeLoc)
LW(ObjCObjectTypeLoc)
LW(PackExpansionTypeLoc)
LW(ParenTypeLoc)
LW(PointerTypeLoc)
LW(QualifiedTypeLoc)
LW(RValueReferenceTypeLoc)
LW(RecordTypeLoc)
LW(ReferenceTypeLoc)
LW(SubstTemplateTypeParmPackTypeLoc)
LW(SubstTemplateTypeParmTypeLoc)
LW(TagTypeLoc)
LW(TemplateSpecializationTypeLoc)
LW(TemplateTypeParmTypeLoc)
LW(TypeLoc)
LW(TypeOfExprTypeLoc)
LW(TypeOfTypeLoc)
LW(TypedefTypeLoc)
LW(UnaryTransformTypeLoc)
LW(UnqualTypeLoc)
LW(UnresolvedUsingTypeLoc)
LW(VariableArrayTypeLoc)
LW(VectorTypeLoc)
PT(AMDGPUNumSGPRAttr)
PT(AMDGPUNumVGPRAttr)
PT(ARMInterruptAttr)
PT(AccessSpecDecl)
PT(AcquireCapabilityAttr)
PT(AcquiredAfterAttr)
PT(AcquiredBeforeAttr)
PT(AddrLabelExpr)
PT(AdjustedType)
PT(AliasAttr)
PT(AlignMac68kAttr)
PT(AlignValueAttr)
PT(AlignedAttr)
PT(AlwaysInlineAttr)
PT(AnalyzerNoReturnAttr)
PT(AnnotateAttr)
PT(ArcWeakrefUnavailableAttr)
PT(ArgumentWithTypeTagAttr)
PT(ArraySubscriptExpr)
PT(ArrayTypeTraitExpr)
PT(AsTypeExpr)
PT(AsmLabelAttr)
PT(AssertCapabilityAttr)
PT(AssertExclusiveLockAttr)
PT(AssertSharedLockAttr)
PT(AssumeAlignedAttr)
PT(AtomicExpr)
PT(AtomicType)
PT(Attr)
PT(AttributedStmt)
PT(AttributedType)
PT(AutoType)
PT(AvailabilityAttr)
PT(BinaryConditionalOperator)
PT(BinaryOperator)
PT(BlockDecl)
PT(BlockExpr)
PT(BlockPointerType)
PT(BlocksAttr)
PT(BreakStmt)
PT(BuiltinType)
PT(C11NoReturnAttr)
PT(CDeclAttr)
PT(CFAuditedTransferAttr)
PT(CFConsumedAttr)
PT(CFReturnsNotRetainedAttr)
PT(CFReturnsRetainedAttr)
PT(CFUnknownTransferAttr)
PT(CStyleCastExpr)
PT(CUDAConstantAttr)
PT(CUDADeviceAttr)
PT(CUDAGlobalAttr)
PT(CUDAHostAttr)
PT(CUDAInvalidTargetAttr)
PT(CUDAKernelCallExpr)
PT(CUDALaunchBoundsAttr)
PT(CUDASharedAttr)
PT(CXX11NoReturnAttr)
PT(CXXBindTemporaryExpr)
PT(CXXBoolLiteralExpr)
PT(CXXCatchStmt)
PT(CXXConstCastExpr)
PT(CXXConstructExpr)
PT(CXXConstructorDecl)
PT(CXXConversionDecl)
PT(CXXDefaultArgExpr)
PT(CXXDefaultInitExpr)
PT(CXXDeleteExpr)
PT(CXXDependentScopeMemberExpr)
PT(CXXDestructorDecl)
PT(CXXDynamicCastExpr)
PT(CXXFoldExpr)
PT(CXXForRangeStmt)
PT(CXXFunctionalCastExpr)
PT(CXXMemberCallExpr)
PT(CXXMethodDecl)
PT(CXXNewExpr)
PT(CXXNoexceptExpr)
PT(CXXNullPtrLiteralExpr)
PT(CXXOperatorCallExpr)
PT(CXXPseudoDestructorExpr)
PT(CXXRecordDecl)
PT(CXXReinterpretCastExpr)
PT(CXXScalarValueInitExpr)
PT(CXXStaticCastExpr)
PT(CXXStdInitializerListExpr)
PT(CXXTemporaryObjectExpr)
PT(CXXThisExpr)
PT(CXXThrowExpr)
PT(CXXTryStmt)
PT(CXXTypeidExpr)
PT(CXXUnresolvedConstructExpr)
PT(CXXUuidofExpr)
PT(CallExpr)
PT(CallableWhenAttr)
PT(CapabilityAttr)
PT(CapturedDecl)
PT(CapturedRecordAttr)
PT(CapturedStmt)
PT(CarriesDependencyAttr)
PT(CaseStmt)
PT(CharacterLiteral)
PT(ChooseExpr)
PT(ClassScopeFunctionSpecializationDecl)
PT(ClassTemplateDecl)
PT(ClassTemplatePartialSpecializationDecl)
PT(ClassTemplateSpecializationDecl)
PT(CleanupAttr)
PT(ColdAttr)
PT(CommonAttr)
PT(ComplexType)
PT(CompoundAssignOperator)
PT(CompoundLiteralExpr)
PT(CompoundStmt)
PT(ConditionalOperator)
PT(ConstAttr)
PT(ConstantArrayType)
PT(ConstructorAttr)
PT(ConsumableAttr)
PT(ConsumableAutoCastAttr)
PT(ConsumableSetOnReadAttr)
PT(ContinueStmt)
PT(ConvertVectorExpr)
PT(DLLExportAttr)
PT(DLLImportAttr)
PT(DecayedType)
PT(Decl)
PT(DeclRefExpr)
PT(DeclStmt)
PT(DecltypeType)
PT(DefaultStmt)
PT(DependentNameType)
PT(DependentScopeDeclRefExpr)
PT(DependentSizedArrayType)
PT(DependentSizedExtVectorType)
PT(DependentTemplateSpecializationType)
PT(DeprecatedAttr)
PT(DesignatedInitExpr)
PT(DestructorAttr)
PT(DoStmt)
PT(ElaboratedType)
PT(EmptyDecl)
PT(EnableIfAttr)
PT(EnumConstantDecl)
PT(EnumDecl)
PT(EnumType)
PT(ExclusiveTrylockFunctionAttr)
PT(ExprWithCleanups)
PT(ExpressionTraitExpr)
PT(ExtVectorElementExpr)
PT(ExtVectorType)
PT(FallThroughAttr)
PT(FastCallAttr)
PT(FieldDecl)
PT(FileScopeAsmDecl)
PT(FinalAttr)
PT(FlattenAttr)
PT(FloatingLiteral)
PT(ForStmt)
PT(FormatArgAttr)
PT(FormatAttr)
PT(FriendDecl)
PT(FriendTemplateDecl)
PT(FunctionDecl)
PT(FunctionNoProtoType)
PT(FunctionParmPackExpr)
PT(FunctionProtoType)
PT(FunctionTemplateDecl)
PT(GCCAsmStmt)
PT(GNUInlineAttr)
PT(GNUNullExpr)
PT(GenericSelectionExpr)
PT(GotoStmt)
PT(GuardedByAttr)
PT(GuardedVarAttr)
PT(HotAttr)
PT(IBActionAttr)
PT(IBOutletAttr)
PT(IBOutletCollectionAttr)
PT(IfStmt)
PT(ImaginaryLiteral)
PT(ImplicitCastExpr)
PT(ImplicitParamDecl)
PT(ImplicitValueInitExpr)
PT(ImportDecl)
PT(IncompleteArrayType)
PT(IndirectFieldDecl)
PT(IndirectGotoStmt)
PT(InitListExpr)
PT(InitPriorityAttr)
PT(InitSegAttr)
PT(InjectedClassNameType)
PT(IntegerLiteral)
PT(IntelOclBiccAttr)
PT(LValueReferenceType)
PT(LabelDecl)
PT(LabelStmt)
PT(LambdaExpr)
PT(LinkageSpecDecl)
PT(LockReturnedAttr)
PT(LocksExcludedAttr)
PT(LoopHintAttr)
PT(MSABIAttr)
PT(MSAsmStmt)
PT(MSDependentExistsStmt)
PT(MSInheritanceAttr)
PT(MSP430InterruptAttr)
PT(MSPropertyDecl)
PT(MSPropertyRefExpr)
PT(MSVtorDispAttr)
PT(MallocAttr)
PT(MaterializeTemporaryExpr)
PT(MaxFieldAlignmentAttr)
PT(MayAliasAttr)
PT(MemberExpr)
PT(MemberPointerType)
PT(MinSizeAttr)
PT(Mips16Attr)
PT(ModeAttr)
PT(MsStructAttr)
PT(NSConsumedAttr)
PT(NSConsumesSelfAttr)
PT(NSReturnsAutoreleasedAttr)
PT(NSReturnsNotRetainedAttr)
PT(NSReturnsRetainedAttr)
PT(NakedAttr)
PT(NamespaceAliasDecl)
PT(NamespaceDecl)
PT(NestedNameSpecifier)
PT(NoCommonAttr)
PT(NoDebugAttr)
PT(NoDuplicateAttr)
PT(NoInlineAttr)
PT(NoInstrumentFunctionAttr)
PT(NoMips16Attr)
PT(NoReturnAttr)
PT(NoSanitizeAddressAttr)
PT(NoSanitizeMemoryAttr)
PT(NoSanitizeThreadAttr)
PT(NoSplitStackAttr)
PT(NoThreadSafetyAnalysisAttr)
PT(NoThrowAttr)
PT(NonNullAttr)
PT(NonTypeTemplateParmDecl)
PT(NullStmt)
PT(OMPAtomicDirective)
PT(OMPBarrierDirective)
PT(OMPCriticalDirective)
PT(OMPFlushDirective)
PT(OMPForDirective)
PT(OMPForSimdDirective)
PT(OMPMasterDirective)
PT(OMPOrderedDirective)
PT(OMPParallelDirective)
PT(OMPParallelForDirective)
PT(OMPParallelForSimdDirective)
PT(OMPParallelSectionsDirective)
PT(OMPSectionDirective)
PT(OMPSectionsDirective)
PT(OMPSimdDirective)
PT(OMPSingleDirective)
PT(OMPTargetDirective)
PT(OMPTaskDirective)
PT(OMPTaskwaitDirective)
PT(OMPTaskyieldDirective)
PT(OMPTeamsDirective)
PT(OMPThreadPrivateDecl)
PT(OMPThreadPrivateDeclAttr)
PT(ObjCArrayLiteral)
PT(ObjCAtCatchStmt)
PT(ObjCAtDefsFieldDecl)
PT(ObjCAtFinallyStmt)
PT(ObjCAtSynchronizedStmt)
PT(ObjCAtThrowStmt)
PT(ObjCAtTryStmt)
PT(ObjCAutoreleasePoolStmt)
PT(ObjCBoolLiteralExpr)
PT(ObjCBoxedExpr)
PT(ObjCBridgeAttr)
PT(ObjCBridgeMutableAttr)
PT(ObjCBridgeRelatedAttr)
PT(ObjCBridgedCastExpr)
PT(ObjCCategoryDecl)
PT(ObjCCategoryImplDecl)
PT(ObjCCompatibleAliasDecl)
PT(ObjCDesignatedInitializerAttr)
PT(ObjCDictionaryLiteral)
PT(ObjCEncodeExpr)
PT(ObjCExceptionAttr)
PT(ObjCExplicitProtocolImplAttr)
PT(ObjCForCollectionStmt)
PT(ObjCImplementationDecl)
PT(ObjCIndirectCopyRestoreExpr)
PT(ObjCInterfaceDecl)
PT(ObjCInterfaceType)
PT(ObjCIsaExpr)
PT(ObjCIvarDecl)
PT(ObjCIvarRefExpr)
PT(ObjCMessageExpr)
PT(ObjCMethodDecl)
PT(ObjCMethodFamilyAttr)
PT(ObjCNSObjectAttr)
PT(ObjCObjectPointerType)
PT(ObjCObjectType)
PT(ObjCPreciseLifetimeAttr)
PT(ObjCPropertyDecl)
PT(ObjCPropertyImplDecl)
PT(ObjCPropertyRefExpr)
PT(ObjCProtocolDecl)
PT(ObjCProtocolExpr)
PT(ObjCRequiresPropertyDefsAttr)
PT(ObjCRequiresSuperAttr)
PT(ObjCReturnsInnerPointerAttr)
PT(ObjCRootClassAttr)
PT(ObjCRuntimeNameAttr)
PT(ObjCSelectorExpr)
PT(ObjCStringLiteral)
PT(ObjCSubscriptRefExpr)
PT(OffsetOfExpr)
PT(OpaqueValueExpr)
PT(OpenCLImageAccessAttr)
PT(OpenCLKernelAttr)
PT(OptimizeNoneAttr)
PT(OverloadableAttr)
PT(OverrideAttr)
PT(OwnershipAttr)
PT(PackExpansionExpr)
PT(PackExpansionType)
PT(PackedAttr)
PT(ParamTypestateAttr)
PT(ParenExpr)
PT(ParenListExpr)
PT(ParenType)
PT(ParmVarDecl)
PT(PascalAttr)
PT(PcsAttr)
PT(PnaclCallAttr)
PT(PointerType)
PT(PredefinedExpr)
PT(PseudoObjectExpr)
PT(PtGuardedByAttr)
PT(PtGuardedVarAttr)
PT(PureAttr)
PT(RValueReferenceType)
PT(RecordDecl)
PT(RecordType)
PT(ReleaseCapabilityAttr)
PT(ReqdWorkGroupSizeAttr)
PT(RequiresCapabilityAttr)
PT(ReturnStmt)
PT(ReturnTypestateAttr)
PT(ReturnsNonNullAttr)
PT(ReturnsTwiceAttr)
PT(SEHExceptStmt)
PT(SEHFinallyStmt)
PT(SEHLeaveStmt)
PT(SEHTryStmt)
PT(ScopedLockableAttr)
PT(SectionAttr)
PT(SelectAnyAttr)
PT(SentinelAttr)
PT(SetTypestateAttr)
PT(SharedTrylockFunctionAttr)
PT(ShuffleVectorExpr)
PT(SizeOfPackExpr)
PT(StaticAssertDecl)
PT(StdCallAttr)
PT(Stmt)
PT(StmtExpr)
PT(StringLiteral)
PT(SubstNonTypeTemplateParmExpr)
PT(SubstNonTypeTemplateParmPackExpr)
PT(SubstTemplateTypeParmPackType)
PT(SubstTemplateTypeParmType)
PT(SwitchStmt)
PT(SysVABIAttr)
PT(TLSModelAttr)
PT(TemplateSpecializationType)
PT(TemplateTemplateParmDecl)
PT(TemplateTypeParmDecl)
PT(TemplateTypeParmType)
PT(TestTypestateAttr)
PT(ThisCallAttr)
PT(ThreadAttr)
PT(TranslationUnitDecl)
PT(TransparentUnionAttr)
PT(TryAcquireCapabilityAttr)
PT(TypeAliasDecl)
PT(TypeAliasTemplateDecl)
PT(TypeOfExprType)
PT(TypeOfType)
PT(TypeTagForDatatypeAttr)
PT(TypeTraitExpr)
PT(TypeVisibilityAttr)
PT(TypedefDecl)
PT(TypedefType)
PT(TypoExpr)
PT(UnaryExprOrTypeTraitExpr)
PT(UnaryOperator)
PT(UnaryTransformType)
PT(UnavailableAttr)
PT(UnresolvedLookupExpr)
PT(UnresolvedMemberExpr)
PT(UnresolvedUsingType)
PT(UnresolvedUsingTypenameDecl)
PT(UnresolvedUsingValueDecl)
PT(UnusedAttr)
PT(UsedAttr)
PT(UserDefinedLiteral)
PT(UsingDecl)
PT(UsingDirectiveDecl)
PT(UsingShadowDecl)
PT(UuidAttr)
PT(VAArgExpr)
PT(VarDecl)
PT(VarTemplateDecl)
PT(VarTemplatePartialSpecializationDecl)
PT(VarTemplateSpecializationDecl)
PT(VariableArrayType)
PT(VecReturnAttr)
PT(VecTypeHintAttr)
PT(VectorCallAttr)
PT(VectorType)
PT(VisibilityAttr)
PT(WarnUnusedAttr)
PT(WarnUnusedResultAttr)
PT(WeakAttr)
PT(WeakImportAttr)
PT(WeakRefAttr)
PT(WhileStmt)
PT(WorkGroupSizeHintAttr)
PT(X86ForceAlignArgPointerAttr)
PV(AMDGPUNumSGPRAttr)
PV(AMDGPUNumVGPRAttr)
PV(ARMInterruptAttr)
PV(AbstractConditionalOperator)
PV(AccessSpecDecl)
PV(AcquireCapabilityAttr)
PV(AcquiredAfterAttr)
PV(AcquiredBeforeAttr)
PV(AddrLabelExpr)
PV(AdjustedType)
PV(AliasAttr)
PV(AlignMac68kAttr)
PV(AlignValueAttr)
PV(AlignedAttr)
PV(AlwaysInlineAttr)
PV(AnalyzerNoReturnAttr)
PV(AnnotateAttr)
PV(ArcWeakrefUnavailableAttr)
PV(ArgumentWithTypeTagAttr)
PV(ArraySubscriptExpr)
PV(ArrayType)
PV(ArrayTypeTraitExpr)
PV(AsTypeExpr)
PV(AsmLabelAttr)
PV(AsmStmt)
PV(AssertCapabilityAttr)
PV(AssertExclusiveLockAttr)
PV(AssertSharedLockAttr)
PV(AssumeAlignedAttr)
PV(AtomicExpr)
PV(AtomicType)
PV(Attr)
PV(AttributedStmt)
PV(AttributedType)
PV(AutoType)
PV(AvailabilityAttr)
PV(BinaryConditionalOperator)
PV(BinaryOperator)
PV(BlockDecl)
PV(BlockExpr)
PV(BlockPointerType)
PV(BlocksAttr)
PV(BreakStmt)
PV(BuiltinType)
PV(C11NoReturnAttr)
PV(CDeclAttr)
PV(CFAuditedTransferAttr)
PV(CFConsumedAttr)
PV(CFReturnsNotRetainedAttr)
PV(CFReturnsRetainedAttr)
PV(CFUnknownTransferAttr)
PV(CStyleCastExpr)
PV(CUDAConstantAttr)
PV(CUDADeviceAttr)
PV(CUDAGlobalAttr)
PV(CUDAHostAttr)
PV(CUDAInvalidTargetAttr)
PV(CUDAKernelCallExpr)
PV(CUDALaunchBoundsAttr)
PV(CUDASharedAttr)
PV(CXX11NoReturnAttr)
PV(CXXBindTemporaryExpr)
PV(CXXBoolLiteralExpr)
PV(CXXCatchStmt)
PV(CXXConstCastExpr)
PV(CXXConstructExpr)
PV(CXXConstructorDecl)
PV(CXXConversionDecl)
PV(CXXDefaultArgExpr)
PV(CXXDefaultInitExpr)
PV(CXXDeleteExpr)
PV(CXXDependentScopeMemberExpr)
PV(CXXDestructorDecl)
PV(CXXDynamicCastExpr)
PV(CXXFoldExpr)
PV(CXXForRangeStmt)
PV(CXXFunctionalCastExpr)
PV(CXXMemberCallExpr)
PV(CXXMethodDecl)
PV(CXXNamedCastExpr)
PV(CXXNewExpr)
PV(CXXNoexceptExpr)
PV(CXXNullPtrLiteralExpr)
PV(CXXOperatorCallExpr)
PV(CXXPseudoDestructorExpr)
PV(CXXRecordDecl)
PV(CXXReinterpretCastExpr)
PV(CXXScalarValueInitExpr)
PV(CXXStaticCastExpr)
PV(CXXStdInitializerListExpr)
PV(CXXTemporaryObjectExpr)
PV(CXXThisExpr)
PV(CXXThrowExpr)
PV(CXXTryStmt)
PV(CXXTypeidExpr)
PV(CXXUnresolvedConstructExpr)
PV(CXXUuidofExpr)
PV(CallExpr)
PV(CallableWhenAttr)
PV(CapabilityAttr)
PV(CapturedDecl)
PV(CapturedRecordAttr)
PV(CapturedStmt)
PV(CarriesDependencyAttr)
PV(CaseStmt)
PV(CastExpr)
PV(CharacterLiteral)
PV(ChooseExpr)
PV(ClassScopeFunctionSpecializationDecl)
PV(ClassTemplateDecl)
PV(ClassTemplatePartialSpecializationDecl)
PV(ClassTemplateSpecializationDecl)
PV(CleanupAttr)
PV(ColdAttr)
PV(CommonAttr)
PV(ComplexType)
PV(CompoundAssignOperator)
PV(CompoundLiteralExpr)
PV(CompoundStmt)
PV(ConditionalOperator)
PV(ConstAttr)
PV(ConstantArrayType)
PV(ConstructorAttr)
PV(ConsumableAttr)
PV(ConsumableAutoCastAttr)
PV(ConsumableSetOnReadAttr)
PV(ContinueStmt)
PV(ConvertVectorExpr)
PV(DLLExportAttr)
PV(DLLImportAttr)
PV(DecayedType)
PV(Decl)
PV(DeclRefExpr)
PV(DeclStmt)
PV(DeclaratorDecl)
PV(DecltypeType)
PV(DefaultStmt)
PV(DependentNameType)
PV(DependentScopeDeclRefExpr)
PV(DependentSizedArrayType)
PV(DependentSizedExtVectorType)
PV(DependentTemplateSpecializationType)
PV(DeprecatedAttr)
PV(DesignatedInitExpr)
PV(DestructorAttr)
PV(DoStmt)
PV(ElaboratedType)
PV(EmptyDecl)
PV(EnableIfAttr)
PV(EnumConstantDecl)
PV(EnumDecl)
PV(EnumType)
PV(ExclusiveTrylockFunctionAttr)
PV(ExplicitCastExpr)
PV(Expr)
PV(ExprWithCleanups)
PV(ExpressionTraitExpr)
PV(ExtVectorElementExpr)
PV(ExtVectorType)
PV(FallThroughAttr)
PV(FastCallAttr)
PV(FieldDecl)
PV(FileScopeAsmDecl)
PV(FinalAttr)
PV(FlattenAttr)
PV(FloatingLiteral)
PV(ForStmt)
PV(FormatArgAttr)
PV(FormatAttr)
PV(FriendDecl)
PV(FriendTemplateDecl)
PV(FunctionDecl)
PV(FunctionNoProtoType)
PV(FunctionParmPackExpr)
PV(FunctionProtoType)
PV(FunctionTemplateDecl)
PV(FunctionType)
PV(GCCAsmStmt)
PV(GNUInlineAttr)
PV(GNUNullExpr)
PV(GenericSelectionExpr)
PV(GotoStmt)
PV(GuardedByAttr)
PV(GuardedVarAttr)
PV(HotAttr)
PV(IBActionAttr)
PV(IBOutletAttr)
PV(IBOutletCollectionAttr)
PV(IfStmt)
PV(ImaginaryLiteral)
PV(ImplicitCastExpr)
PV(ImplicitParamDecl)
PV(ImplicitValueInitExpr)
PV(ImportDecl)
PV(IncompleteArrayType)
PV(IndirectFieldDecl)
PV(IndirectGotoStmt)
PV(InitListExpr)
PV(InitPriorityAttr)
PV(InitSegAttr)
PV(InjectedClassNameType)
PV(IntegerLiteral)
PV(IntelOclBiccAttr)
PV(LValueReferenceType)
PV(LabelDecl)
PV(LabelStmt)
PV(LambdaExpr)
PV(LinkageSpecDecl)
PV(LockReturnedAttr)
PV(LocksExcludedAttr)
PV(LoopHintAttr)
PV(MSABIAttr)
PV(MSAsmStmt)
PV(MSDependentExistsStmt)
PV(MSInheritanceAttr)
PV(MSP430InterruptAttr)
PV(MSPropertyDecl)
PV(MSPropertyRefExpr)
PV(MSVtorDispAttr)
PV(MallocAttr)
PV(MaterializeTemporaryExpr)
PV(MaxFieldAlignmentAttr)
PV(MayAliasAttr)
PV(MemberExpr)
PV(MemberPointerType)
PV(MinSizeAttr)
PV(Mips16Attr)
PV(ModeAttr)
PV(MsStructAttr)
PV(NSConsumedAttr)
PV(NSConsumesSelfAttr)
PV(NSReturnsAutoreleasedAttr)
PV(NSReturnsNotRetainedAttr)
PV(NSReturnsRetainedAttr)
PV(NakedAttr)
PV(NamedDecl)
PV(NamespaceAliasDecl)
PV(NamespaceDecl)
PV(NoCommonAttr)
PV(NoDebugAttr)
PV(NoDuplicateAttr)
PV(NoInlineAttr)
PV(NoInstrumentFunctionAttr)
PV(NoMips16Attr)
PV(NoReturnAttr)
PV(NoSanitizeAddressAttr)
PV(NoSanitizeMemoryAttr)
PV(NoSanitizeThreadAttr)
PV(NoSplitStackAttr)
PV(NoThreadSafetyAnalysisAttr)
PV(NoThrowAttr)
PV(NonNullAttr)
PV(NonTypeTemplateParmDecl)
PV(NullStmt)
PV(OMPAtomicDirective)
PV(OMPBarrierDirective)
PV(OMPCriticalDirective)
PV(OMPExecutableDirective)
PV(OMPFlushDirective)
PV(OMPForDirective)
PV(OMPForSimdDirective)
PV(OMPLoopDirective)
PV(OMPMasterDirective)
PV(OMPOrderedDirective)
PV(OMPParallelDirective)
PV(OMPParallelForDirective)
PV(OMPParallelForSimdDirective)
PV(OMPParallelSectionsDirective)
PV(OMPSectionDirective)
PV(OMPSectionsDirective)
PV(OMPSimdDirective)
PV(OMPSingleDirective)
PV(OMPTargetDirective)
PV(OMPTaskDirective)
PV(OMPTaskwaitDirective)
PV(OMPTaskyieldDirective)
PV(OMPTeamsDirective)
PV(OMPThreadPrivateDecl)
PV(OMPThreadPrivateDeclAttr)
PV(ObjCArrayLiteral)
PV(ObjCAtCatchStmt)
PV(ObjCAtDefsFieldDecl)
PV(ObjCAtFinallyStmt)
PV(ObjCAtSynchronizedStmt)
PV(ObjCAtThrowStmt)
PV(ObjCAtTryStmt)
PV(ObjCAutoreleasePoolStmt)
PV(ObjCBoolLiteralExpr)
PV(ObjCBoxedExpr)
PV(ObjCBridgeAttr)
PV(ObjCBridgeMutableAttr)
PV(ObjCBridgeRelatedAttr)
PV(ObjCBridgedCastExpr)
PV(ObjCCategoryDecl)
PV(ObjCCategoryImplDecl)
PV(ObjCCompatibleAliasDecl)
PV(ObjCContainerDecl)
PV(ObjCDesignatedInitializerAttr)
PV(ObjCDictionaryLiteral)
PV(ObjCEncodeExpr)
PV(ObjCExceptionAttr)
PV(ObjCExplicitProtocolImplAttr)
PV(ObjCForCollectionStmt)
PV(ObjCImplDecl)
PV(ObjCImplementationDecl)
PV(ObjCIndirectCopyRestoreExpr)
PV(ObjCInterfaceDecl)
PV(ObjCInterfaceType)
PV(ObjCIsaExpr)
PV(ObjCIvarDecl)
PV(ObjCIvarRefExpr)
PV(ObjCMessageExpr)
PV(ObjCMethodDecl)
PV(ObjCMethodFamilyAttr)
PV(ObjCNSObjectAttr)
PV(ObjCObjectPointerType)
PV(ObjCObjectType)
PV(ObjCPreciseLifetimeAttr)
PV(ObjCPropertyDecl)
PV(ObjCPropertyImplDecl)
PV(ObjCPropertyRefExpr)
PV(ObjCProtocolDecl)
PV(ObjCProtocolExpr)
PV(ObjCRequiresPropertyDefsAttr)
PV(ObjCRequiresSuperAttr)
PV(ObjCReturnsInnerPointerAttr)
PV(ObjCRootClassAttr)
PV(ObjCRuntimeNameAttr)
PV(ObjCSelectorExpr)
PV(ObjCStringLiteral)
PV(ObjCSubscriptRefExpr)
PV(OffsetOfExpr)
PV(OpaqueValueExpr)
PV(OpenCLImageAccessAttr)
PV(OpenCLKernelAttr)
PV(OptimizeNoneAttr)
PV(OverloadExpr)
PV(OverloadableAttr)
PV(OverrideAttr)
PV(OwnershipAttr)
PV(PackExpansionExpr)
PV(PackExpansionType)
PV(PackedAttr)
PV(ParamTypestateAttr)
PV(ParenExpr)
PV(ParenListExpr)
PV(ParenType)
PV(ParmVarDecl)
PV(PascalAttr)
PV(PcsAttr)
PV(PnaclCallAttr)
PV(PointerType)
PV(PredefinedExpr)
PV(PseudoObjectExpr)
PV(PtGuardedByAttr)
PV(PtGuardedVarAttr)
PV(PureAttr)
PV(RValueReferenceType)
PV(RecordDecl)
PV(RecordType)
PV(RedeclarableTemplateDecl)
PV(ReferenceType)
PV(ReleaseCapabilityAttr)
PV(ReqdWorkGroupSizeAttr)
PV(RequiresCapabilityAttr)
PV(ReturnStmt)
PV(ReturnTypestateAttr)
PV(ReturnsNonNullAttr)
PV(ReturnsTwiceAttr)
PV(SEHExceptStmt)
PV(SEHFinallyStmt)
PV(SEHLeaveStmt)
PV(SEHTryStmt)
PV(ScopedLockableAttr)
PV(SectionAttr)
PV(SelectAnyAttr)
PV(SentinelAttr)
PV(SetTypestateAttr)
PV(SharedTrylockFunctionAttr)
PV(ShuffleVectorExpr)
PV(SizeOfPackExpr)
PV(StaticAssertDecl)
PV(StdCallAttr)
PV(Stmt)
PV(StmtExpr)
PV(StringLiteral)
PV(SubstNonTypeTemplateParmExpr)
PV(SubstNonTypeTemplateParmPackExpr)
PV(SubstTemplateTypeParmPackType)
PV(SubstTemplateTypeParmType)
PV(SwitchCase)
PV(SwitchStmt)
PV(SysVABIAttr)
PV(TLSModelAttr)
PV(TagDecl)
PV(TagType)
PV(TemplateDecl)
PV(TemplateSpecializationType)
PV(TemplateTemplateParmDecl)
PV(TemplateTypeParmDecl)
PV(TemplateTypeParmType)
PV(TestTypestateAttr)
PV(ThisCallAttr)
PV(ThreadAttr)
PV(TranslationUnitDecl)
PV(TransparentUnionAttr)
PV(TryAcquireCapabilityAttr)
PV(Type)
PV(TypeAliasDecl)
PV(TypeAliasTemplateDecl)
PV(TypeDecl)
PV(TypeOfExprType)
PV(TypeOfType)
PV(TypeTagForDatatypeAttr)
PV(TypeTraitExpr)
PV(TypeVisibilityAttr)
PV(TypedefDecl)
PV(TypedefNameDecl)
PV(TypedefType)
PV(TypoExpr)
PV(UnaryExprOrTypeTraitExpr)
PV(UnaryOperator)
PV(UnaryTransformType)
PV(UnavailableAttr)
PV(UnresolvedLookupExpr)
PV(UnresolvedMemberExpr)
PV(UnresolvedUsingType)
PV(UnresolvedUsingTypenameDecl)
PV(UnresolvedUsingValueDecl)
PV(UnusedAttr)
PV(UsedAttr)
PV(UserDefinedLiteral)
PV(UsingDecl)
PV(UsingDirectiveDecl)
PV(UsingShadowDecl)
PV(UuidAttr)
PV(VAArgExpr)
PV(ValueDecl)
PV(VarDecl)
PV(VarTemplateDecl)
PV(VarTemplatePartialSpecializationDecl)
PV(VarTemplateSpecializationDecl)
PV(VariableArrayType)
PV(VecReturnAttr)
PV(VecTypeHintAttr)
PV(VectorCallAttr)
PV(VectorType)
PV(VisibilityAttr)
PV(WarnUnusedAttr)
PV(WarnUnusedResultAttr)
PV(WeakAttr)
PV(WeakImportAttr)
PV(WeakRefAttr)
PV(WhileStmt)
PV(WorkGroupSizeHintAttr)
PV(X86ForceAlignArgPointerAttr)
PW(AbstractConditionalOperator)
PW(AccessSpecDecl)
PW(AddrLabelExpr)
PW(AdjustedType)
PW(ArraySubscriptExpr)
PW(ArrayType)
PW(ArrayTypeTraitExpr)
PW(AsTypeExpr)
PW(AsmStmt)
PW(AtomicExpr)
PW(AtomicType)
PW(AttributedStmt)
PW(AttributedType)
PW(AutoType)
PW(BinaryConditionalOperator)
PW(BinaryOperator)
PW(BlockDecl)
PW(BlockExpr)
PW(BlockPointerType)
PW(BreakStmt)
PW(BuiltinType)
PW(CStyleCastExpr)
PW(CUDAKernelCallExpr)
PW(CXXBindTemporaryExpr)
PW(CXXBoolLiteralExpr)
PW(CXXCatchStmt)
PW(CXXConstCastExpr)
PW(CXXConstructExpr)
PW(CXXConstructorDecl)
PW(CXXConversionDecl)
PW(CXXDefaultArgExpr)
PW(CXXDefaultInitExpr)
PW(CXXDeleteExpr)
PW(CXXDependentScopeMemberExpr)
PW(CXXDestructorDecl)
PW(CXXDynamicCastExpr)
PW(CXXFoldExpr)
PW(CXXForRangeStmt)
PW(CXXFunctionalCastExpr)
PW(CXXMemberCallExpr)
PW(CXXMethodDecl)
PW(CXXNamedCastExpr)
PW(CXXNewExpr)
PW(CXXNoexceptExpr)
PW(CXXNullPtrLiteralExpr)
PW(CXXOperatorCallExpr)
PW(CXXPseudoDestructorExpr)
PW(CXXRecordDecl)
PW(CXXReinterpretCastExpr)
PW(CXXScalarValueInitExpr)
PW(CXXStaticCastExpr)
PW(CXXStdInitializerListExpr)
PW(CXXTemporaryObjectExpr)
PW(CXXThisExpr)
PW(CXXThrowExpr)
PW(CXXTryStmt)
PW(CXXTypeidExpr)
PW(CXXUnresolvedConstructExpr)
PW(CXXUuidofExpr)
PW(CallExpr)
PW(CapturedDecl)
PW(CapturedStmt)
PW(CaseStmt)
PW(CastExpr)
PW(CharacterLiteral)
PW(ChooseExpr)
PW(ClassScopeFunctionSpecializationDecl)
PW(ClassTemplateDecl)
PW(ClassTemplatePartialSpecializationDecl)
PW(ClassTemplateSpecializationDecl)
PW(ComplexType)
PW(CompoundAssignOperator)
PW(CompoundLiteralExpr)
PW(CompoundStmt)
PW(ConditionalOperator)
PW(ConstantArrayType)
PW(ContinueStmt)
PW(ConvertVectorExpr)
PW(DecayedType)
PW(Decl)
PW(DeclRefExpr)
PW(DeclStmt)
PW(DeclaratorDecl)
PW(DecltypeType)
PW(DefaultStmt)
PW(DependentNameType)
PW(DependentScopeDeclRefExpr)
PW(DependentSizedArrayType)
PW(DependentSizedExtVectorType)
PW(DependentTemplateSpecializationType)
PW(DesignatedInitExpr)
PW(DoStmt)
PW(ElaboratedType)
PW(EmptyDecl)
PW(EnumConstantDecl)
PW(EnumDecl)
PW(EnumType)
PW(ExplicitCastExpr)
PW(Expr)
PW(ExprWithCleanups)
PW(ExpressionTraitExpr)
PW(ExtVectorElementExpr)
PW(ExtVectorType)
PW(FieldDecl)
PW(FileScopeAsmDecl)
PW(FloatingLiteral)
PW(ForStmt)
PW(FriendDecl)
PW(FriendTemplateDecl)
PW(FunctionDecl)
PW(FunctionNoProtoType)
PW(FunctionParmPackExpr)
PW(FunctionProtoType)
PW(FunctionTemplateDecl)
PW(FunctionType)
PW(GCCAsmStmt)
PW(GNUNullExpr)
PW(GenericSelectionExpr)
PW(GotoStmt)
PW(IfStmt)
PW(ImaginaryLiteral)
PW(ImplicitCastExpr)
PW(ImplicitParamDecl)
PW(ImplicitValueInitExpr)
PW(ImportDecl)
PW(IncompleteArrayType)
PW(IndirectFieldDecl)
PW(IndirectGotoStmt)
PW(InitListExpr)
PW(InjectedClassNameType)
PW(IntegerLiteral)
PW(LValueReferenceType)
PW(LabelDecl)
PW(LabelStmt)
PW(LambdaExpr)
PW(LinkageSpecDecl)
PW(MSAsmStmt)
PW(MSDependentExistsStmt)
PW(MSPropertyDecl)
PW(MSPropertyRefExpr)
PW(MaterializeTemporaryExpr)
PW(MemberExpr)
PW(MemberPointerType)
PW(NamedDecl)
PW(NamespaceAliasDecl)
PW(NamespaceDecl)
PW(NonTypeTemplateParmDecl)
PW(NullStmt)
PW(OMPAtomicDirective)
PW(OMPBarrierDirective)
PW(OMPCriticalDirective)
PW(OMPExecutableDirective)
PW(OMPFlushDirective)
PW(OMPForDirective)
PW(OMPForSimdDirective)
PW(OMPLoopDirective)
PW(OMPMasterDirective)
PW(OMPOrderedDirective)
PW(OMPParallelDirective)
PW(OMPParallelForDirective)
PW(OMPParallelForSimdDirective)
PW(OMPParallelSectionsDirective)
PW(OMPSectionDirective)
PW(OMPSectionsDirective)
PW(OMPSimdDirective)
PW(OMPSingleDirective)
PW(OMPTargetDirective)
PW(OMPTaskDirective)
PW(OMPTaskwaitDirective)
PW(OMPTaskyieldDirective)
PW(OMPTeamsDirective)
PW(OMPThreadPrivateDecl)
PW(ObjCArrayLiteral)
PW(ObjCAtCatchStmt)
PW(ObjCAtDefsFieldDecl)
PW(ObjCAtFinallyStmt)
PW(ObjCAtSynchronizedStmt)
PW(ObjCAtThrowStmt)
PW(ObjCAtTryStmt)
PW(ObjCAutoreleasePoolStmt)
PW(ObjCBoolLiteralExpr)
PW(ObjCBoxedExpr)
PW(ObjCBridgedCastExpr)
PW(ObjCCategoryDecl)
PW(ObjCCategoryImplDecl)
PW(ObjCCompatibleAliasDecl)
PW(ObjCContainerDecl)
PW(ObjCDictionaryLiteral)
PW(ObjCEncodeExpr)
PW(ObjCForCollectionStmt)
PW(ObjCImplDecl)
PW(ObjCImplementationDecl)
PW(ObjCIndirectCopyRestoreExpr)
PW(ObjCInterfaceDecl)
PW(ObjCInterfaceType)
PW(ObjCIsaExpr)
PW(ObjCIvarDecl)
PW(ObjCIvarRefExpr)
PW(ObjCMessageExpr)
PW(ObjCMethodDecl)
PW(ObjCObjectPointerType)
PW(ObjCObjectType)
PW(ObjCPropertyDecl)
PW(ObjCPropertyImplDecl)
PW(ObjCPropertyRefExpr)
PW(ObjCProtocolDecl)
PW(ObjCProtocolExpr)
PW(ObjCSelectorExpr)
PW(ObjCStringLiteral)
PW(ObjCSubscriptRefExpr)
PW(OffsetOfExpr)
PW(OpaqueValueExpr)
PW(OverloadExpr)
PW(PackExpansionExpr)
PW(PackExpansionType)
PW(ParenExpr)
PW(ParenListExpr)
PW(ParenType)
PW(ParmVarDecl)
PW(PointerType)
PW(PredefinedExpr)
PW(PseudoObjectExpr)
PW(RValueReferenceType)
PW(RecordDecl)
PW(RecordType)
PW(RedeclarableTemplateDecl)
PW(ReferenceType)
PW(ReturnStmt)
PW(SEHExceptStmt)
PW(SEHFinallyStmt)
PW(SEHLeaveStmt)
PW(SEHTryStmt)
PW(ShuffleVectorExpr)
PW(SizeOfPackExpr)
PW(StaticAssertDecl)
PW(Stmt)
PW(StmtExpr)
PW(StringLiteral)
PW(SubstNonTypeTemplateParmExpr)
PW(SubstNonTypeTemplateParmPackExpr)
PW(SubstTemplateTypeParmPackType)
PW(SubstTemplateTypeParmType)
PW(SwitchCase)
PW(SwitchStmt)
PW(TagDecl)
PW(TagType)
PW(TemplateDecl)
PW(TemplateSpecializationType)
PW(TemplateTemplateParmDecl)
PW(TemplateTypeParmDecl)
PW(TemplateTypeParmType)
PW(TranslationUnitDecl)
PW(Type)
PW(TypeAliasDecl)
PW(TypeAliasTemplateDecl)
PW(TypeDecl)
PW(TypeOfExprType)
PW(TypeOfType)
PW(TypeTraitExpr)
PW(TypedefDecl)
PW(TypedefNameDecl)
PW(TypedefType)
PW(TypoExpr)
PW(UnaryExprOrTypeTraitExpr)
PW(UnaryOperator)
PW(UnaryTransformType)
PW(UnresolvedLookupExpr)
PW(UnresolvedMemberExpr)
PW(UnresolvedUsingType)
PW(UnresolvedUsingTypenameDecl)
PW(UnresolvedUsingValueDecl)
PW(UserDefinedLiteral)
PW(UsingDecl)
PW(UsingDirectiveDecl)
PW(UsingShadowDecl)
PW(VAArgExpr)
PW(ValueDecl)
PW(VarDecl)
PW(VarTemplateDecl)
PW(VarTemplatePartialSpecializationDecl)
PW(VarTemplateSpecializationDecl)
PW(VariableArrayType)
PW(VectorType)
PW(WhileStmt)
#endif

#undef  F
#undef  F2
#undef  F2T
#undef  FT
#undef  FV
#undef  FW
#undef  LT
#undef  LV
#undef  LW
#undef  PT
#undef  PV
#undef  PW
};
#endif
