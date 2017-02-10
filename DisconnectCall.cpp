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
#include "DisconnectCall.h"
#include "SlotCall.h"

using namespace std;
using namespace clang;

DisconnectCall::DisconnectCall(Context &context, const CallExpr *expr) :
    SlotCall(context)
{
    FunctionDecl::param_const_iterator it;
    const FunctionDecl *decl;
    const CXXRecordDecl *parent;
    std::string signal, slot;
    unsigned signal_index, slot_index;

    decl = expr->getDirectCallee();

    if (decl == nullptr) {
        return;
    }

    if (decl->getNameAsString().compare("disconnect") != 0) {
        return;
    }

    parent = dyn_cast_or_null<CXXRecordDecl>(decl->getParent());

    if (parent == nullptr) {
        return;
    }

    if (parent->getNameAsString().compare("QObject") != 0) {
        return;
    }

    for (it = decl->param_begin(); it != decl->param_end(); it++) {
        if ((*it)->getType().getAsString().compare("const char *") == 0) {
            break;
        }
    }

    if (it == decl->param_end()) {
        _context.note(expr->getLocStart(), "no string, skipping");
        return;
    }

    if (decl->getNumParams() == 3) {
        const CXXMemberCallExpr *cxx_call;

        cxx_call = dyn_cast_or_null<CXXMemberCallExpr>(expr);

        if (cxx_call == nullptr) {
            _context.note(expr->getLocStart(), "unable to convert into C++ member call");
            return;
        }

        signal = getSignal(cxx_call->getImplicitObjectArgument(), expr->getArg(0));
        signal_index = 0;

        if (!signal.empty()) {
            signal = string("this, ") + signal;
        }

        slot = getSlot(expr->getArg(1), expr->getArg(2));
        slot_index = 2;
    } else if (decl->getNumParams() == 4) {
        signal = getSignal(expr->getArg(0), expr->getArg(1));
        signal_index = 1;
        slot = getSlot(expr->getArg(2), expr->getArg(3));
        slot_index = 3;
    } else {
        return;
    }

    if (signal.empty() || slot.empty()) {
        return;
    }

    replace(expr->getArg(signal_index), signal);
    replace(expr->getArg(slot_index), slot);

    reportMatch();
}

bool DisconnectCall::processCallExpr(Context &context, const clang::CallExpr *expr)
{
    DisconnectCall call(context, expr);
    return call.matches();
}
