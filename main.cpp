#include <string>

#include "clang_headers.h"

#include "Context.h"
#include "SlotCall.h"

using namespace std;
using namespace clang;

namespace {

class FuncCallVisitor : public RecursiveASTVisitor<FuncCallVisitor> {
public:
    explicit FuncCallVisitor(Context &context) : _context(context) {}

    bool VisitCallExpr(const CallExpr *expr) {
        SlotCall::processCallExpr(_context, expr);
        return true;
    }

private:
    Context &_context;
};

class FuncCallConsumer : public ASTConsumer {
public:
    explicit FuncCallConsumer(CompilerInstance &ci) :
        _context(ci),
        _visitor(FuncCallVisitor(_context)) {}

    virtual bool HandleTopLevelDecl(DeclGroupRef dg) override {
        for (Decl *decl : dg) {
            _visitor.TraverseDecl(decl);
        }

        return true;
    }

private:
    Context _context;
    FuncCallVisitor _visitor;
};

class QtReslot : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                                   llvm::StringRef) override {
        return llvm::make_unique<FuncCallConsumer>(ci);
    }

    bool ParseArgs(const CompilerInstance&,
                   const std::vector<std::string>&) override {
        return true;
    }
};

}

static FrontendPluginRegistry::Add<QtReslot>
X("rewrite_signals_and_slots", "rewrite source files with Qt5 signal and slot connections");
