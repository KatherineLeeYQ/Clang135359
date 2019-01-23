[SCC = Strongly connected component](https://en.wikipedia.org/wiki/Strongly_connected_component "SCC")

# [AST介绍](https://stephenwzl.github.io/2018/01/08/clang-ast/ "AST介绍")
![d](https://stephenwzl.github.io/images/ast-dump.png "d")
## TranslationUnit
Clang AST的最顶层结构叫做 **TranslationUnit**，我们管它叫做“编译单元”。
## RecursiveASTVisitor
在拿到 Clang提供的 AST时，其顶层结构肯定是一个“编译单元”，去查找某个节点我们可以进行遍历，但也可以使用 Clang封装好的访问算法，这个算法的封装就是RecursiveASTVisitor。 我们继承这个类，并且实现其中的 VisitCXXRecordDecl，那么这个方法就会在访问 CXXRecordDecl类型的节点上触发。(CXXRecordDecl 类型用于表示 C++ class/union/struct)
## FrontendAction和 ASTConsumer
前面我们已经实现了 RecursiveASTVisitor, 按道理已经可以实现基于 Clang AST查找符号的核心逻辑了，但是很明显我们需要一个入口，这个入口需要定义我们如何通过一个编译实例(compiler instance)获得整个 AST以及如何进行 AST的访问。
FrontendAction就是一个与编译实例打交道的东西，词法分析、语法分析等过程都被编译实例隐藏了，编译实例会触发 FrontendAction定义好的一些方法，并且把编译过程中的详细信息告诉它，比如编译了哪个文件、编译参数等。而 ASTConsumer其实是 FrontendAction的一个子过程产物，在 ASTConsumer中我们便可以拿到整个“编译单元”，调用前面的 Visitor进行访问和查找。

# Clang类
## AST
**AST/ASTContext.h**
**AST/DeclBase.h**
- ASTContext &getASTContext()

## Index
// Brief Maps information to TranslationUnits.
**Index/Indexer.h**

- TranslationUnit *getTU(ASTContext &Ctx)

// Bbrief Abstract interface for a translation unit.
**Index/TransitionUnit.h**
**Index/CallGraph.h**
1. CallGraphNode
2. CallGraph

**Index/Entity.h**

- Decl *getInternalDecl()

## Basic
**Basic/SourceLocation.h**
1. SourceLocation
2. PresumeLoc

**Basic/SourceManager.h**

# CSA
**StaticAnalyzer/Core/Pathsensitive/SummaryManager.h**
bool isAnalyzed(const FunctionDecl *FD)
void MarkAsAnalyzed(const FunctionDecl *FD)
void resetSummary(const FunctionDecl *FD)
void clearAccessPaths(const FunctionDecl *FD)

**StaticAnalyzer/Core/Pathsensitive/AnalysisManager.h**

**StaticAnalyzer/Core/Pathsensitive/ExprEngine.h**

# Melton使用
**ASTUnitTU -> TranslationUnit**
> ASTUnit *AST;
DeclReferenceMap DeclRefMap;
SelectorMap SelMap;

- virtual ASTContext &getASTContext()
- virtual Preprocessor &getPreprocessor()
- virtual Diagnostic &getDiagnostic()
- virtual DeclReferenceMap &getDeclReferenceMap()
- virtual SelectorMap &getSelectorMap()

**CGTraverser::Visit**
- virtual bool visit(CallGraphNode *N, bool repeat, bool flushReports) = 0;

**CGTraverser**
- virtual void TraverseCallGraph(CallGraph &CG, Visitor &V) = 0;
- virtual SummaryManager &getSummaryManager() = 0;
- virtual llvm::SmallVectorImpl<std::string> *getFilesMade() { return NULL; };

**SCCTraverser -> CGTraverser**
> llvm::OwningPtr<SummaryManager> summMgr;
llvm::SmallVectorImpl<std::string> FilesMade;

- SCCTraverser() : FilesMade(50);
- SummaryManager &getSummaryManager();
- llvm::SmallVectorImpl<std::string> *getFilesMade();
- void TraverseCallGraph(CallGraph &CG, Visitor &V);

**SimpleTraverser -> CGTraverser**
> llvm::OwningPtr<SummaryManager> summMgr;

- SummaryManager &getSummaryManager()
- void TraverseCallGraph(CallGraph &CG, Visitor &V)

**WPAVisitor -> CGTraverser::Visitor**
> Indexer &Idxer;
SummaryManager &SummMgr;
llvm::SmallVectorImpl<std::string> *FilesMade;

- bool WPAVisitor::visit(CallGraphNode *N, bool repeat, bool flushReports)
- bool visitEntity(Entity ent, bool repeat = false, bool flushReports = true);

**void SCCDump(CallGraph &CG)**

**TimerRegion**
> llvm::Timer &mTimer;

- 构造函数里开始计时，析构函数里停止计时，并打印时间长度
