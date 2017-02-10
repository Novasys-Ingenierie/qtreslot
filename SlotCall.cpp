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

#include <stdlib.h>

#include <string>

#include "clang_headers.h"

#include "AddActionCall.h"
#include "ConnectCall.h"
#include "DisconnectCall.h"
#include "SingleShotCall.h"
#include "SlotCall.h"

using namespace std;
using namespace clang;

const list<SlotCall::processor> SlotCall::processors = list<SlotCall::processor>({
    AddActionCall::processCallExpr,
    ConnectCall::processCallExpr,
    DisconnectCall::processCallExpr,
    SingleShotCall::processCallExpr,
});

void
SlotCall::processCallExpr(Context &context, const CallExpr *expr)
{
    bool matches;

    for (processor p : processors) {
        matches = p(context, expr);

        if (matches) {
            return;
        }
    }
}

bool
SlotCall::isnotspace(int c)
{
    return !isspace(c);
}

void
SlotCall::ltrim(string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), isnotspace));
}
                                     
void
SlotCall::rtrim(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(), isnotspace).base(), s.end());
}

void
SlotCall::trim(string &s)
{
    ltrim(s);
    rtrim(s);
}

string
SlotCall::getSource(const Expr *expr)
{
    return Lexer::getSourceText(CharSourceRange::getTokenRange(expr->getSourceRange()),
                                _context._ci.getSourceManager(),
                                _context._ci.getLangOpts()).str();
}

string
SlotCall::getArgument(const string &s)
{
    size_t start, end;
    string arg;

    start = s.find("(");

    if (start == string::npos) {
        return "";
    }

    start++;
    end = s.find("(", start);

    if (end == string::npos) {
        return "";
    }

    arg = s.substr(start, end - start);
    trim(arg);
    return arg;
}

string
SlotCall::getRawType(const Expr *expr)
{
    const CXXRecordDecl *decl;

    decl = expr->IgnoreImpCasts()->getType()->getPointeeCXXRecordDecl();

    if (decl == nullptr) {
        return "";
    }

    return decl->getName();
}

bool SlotCall::checkSuppressionFromQtObjectHeader(const Expr *expr)
{
    SourceLocation fileLoc = _context.getSourceMgr().getFileLoc(expr->getLocStart());
    string s = _context.getSourceMgr().getFilename(fileLoc).str();

    // Not as accurate as possible, but should do the job for everyone
    return (s.find("/qobject.h") != string::npos);
}

bool
SlotCall::checkSuppression(const Expr *expr)
{
    if (checkSuppressionFromQtObjectHeader(expr)) {
        return true;
    }

    return false;
}

string
SlotCall::getSignal(const Expr *sender, const Expr *signal)
{
    string source, type, arg, result;
    size_t pos;

    type = getRawType(sender);

    if (type.empty()) {
        _context.warn(sender->getLocStart(), "unable to identify type");
        goto out;
    }

    source = getSource(signal);
    pos = source.find("SIGNAL");

    if (pos == string::npos) {
        if (!checkSuppression(signal)) {
            _context.warn(signal->getLocStart(), "no SIGNAL macro");
        }

        goto out;
    }

    arg = getArgument(source.substr(pos));

    if (arg.empty()) {
        _context.warn(signal->getLocStart(), "malformed SIGNAL macro usage");
        goto out;
    }

    result.append("&").append(type).append("::").append(arg);
    trim(result);

out:
    return result;
}

string
SlotCall::getSlot(const Expr *receiver, const Expr *slot)
{
    string source, type, arg, result;
    size_t pos;

    type = getRawType(receiver);

    if (type.empty()) {
        _context.warn(receiver->getLocStart(), "unable to identify type");
        goto out;
    }

    source = getSource(slot);
    pos = source.find("SLOT");

    if (pos == string::npos) {
        if (!checkSuppression(slot)) {
            _context.warn(slot->getLocStart(), "no SLOT macro");
        }

        goto out;
    }

    arg = getArgument(source.substr(pos));

    if (arg.empty()) {
        _context.warn(slot->getLocStart(), "malformed SLOT macro usage");
        goto out;
    }

    result.append("&").append(type).append("::").append(arg);
    trim(result);

out:
    return result;
}

void
SlotCall::replace(const clang::Expr *expr, const string &s)
{
    pair<SourceLocation, SourceLocation> loc_pair =
        _context.getSourceMgr().getExpansionRange(expr->getLocStart());
    SourceRange range(loc_pair.first, loc_pair.second);
    bool error = _context.ReplaceText(range, s);

    if (error) {
        _context.note(expr->getLocStart(), "unable to transform expression");
    }
}
