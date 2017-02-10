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

#ifndef _SLOTCALL_H
#define _SLOTCALL_H

#include "Context.h"

class SlotCall {
public:
    typedef std::function<bool(Context &, const clang::CallExpr *)> processor;

    virtual ~SlotCall() {}

    static void processCallExpr(Context &context, const clang::CallExpr *expr);

protected:
    Context &_context;

    SlotCall(Context &context) : _context(context), _match(false) {}

    void reportMatch(void) {
        _match = true;
    }

    bool matches() {
        return _match;
    }

    std::string getSource(const clang::Expr *expr);
    std::string getSignal(const clang::Expr *sender, const clang::Expr *signal);
    std::string getSlot(const clang::Expr *receiver, const clang::Expr *slot);
    void replace(const clang::Expr *expr, const std::string &s);

private:
    bool _match;

    static const std::list<processor> processors;

    static bool isnotspace(int c);
    static void ltrim(std::string &s);
    static void rtrim(std::string &s);
    static void trim(std::string &s);

    std::string getArgument(const std::string &s);
    std::string getRawType(const clang::Expr *expr);
    bool checkSuppressionFromQtObjectHeader(const clang::Expr *expr);
    bool checkSuppression(const clang::Expr *expr);
};

#endif // _SLOTCALL_H
