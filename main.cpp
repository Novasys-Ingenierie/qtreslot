/*
 * Copyright (c) 2016 Novasys Ingénierie.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
