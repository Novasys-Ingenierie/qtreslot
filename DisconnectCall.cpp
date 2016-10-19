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
