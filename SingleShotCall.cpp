#include <string>

#include "clang_headers.h"

#include "Context.h"
#include "SingleShotCall.h"
#include "SlotCall.h"

using namespace std;
using namespace clang;

SingleShotCall::SingleShotCall(Context &context, const CallExpr *expr) :
    SlotCall(context)
{
    FunctionDecl::param_const_iterator it;
    const FunctionDecl *decl;
    const CXXRecordDecl *parent;
    std::string slot, type;
    unsigned slot_index;

    decl = expr->getDirectCallee();

    if (decl == nullptr) {
        return;
    }

    if (decl->getNameAsString().compare("singleShot") != 0) {
        return;
    }

    parent = dyn_cast_or_null<CXXRecordDecl>(decl->getParent());

    if (parent == nullptr) {
        return;
    }

    if (parent->getNameAsString().compare("QTimer") != 0) {
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
        slot = getSlot(expr->getArg(1), expr->getArg(2));
        slot_index = 2;
    } else if (decl->getNumParams() == 4) {
        slot = getSlot(expr->getArg(2), expr->getArg(3));
        slot_index = 3;
    } else {
        return;
    }

    if (slot.empty()) {
        return;
    }

    replace(expr->getArg(slot_index), slot);

    reportMatch();
}

bool SingleShotCall::processCallExpr(Context &context, const clang::CallExpr *expr)
{
    SingleShotCall call(context, expr);
    return call.matches();
}
