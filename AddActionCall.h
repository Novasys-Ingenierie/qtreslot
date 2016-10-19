#ifndef _ADDACTIONCALL_H
#define _ADDACTIONCALL_H

#include "Context.h"
#include "SlotCall.h"

class AddActionCall : public SlotCall {
public:
    virtual ~AddActionCall() {}

    static bool processCallExpr(Context &context, const clang::CallExpr *expr);

private:
    AddActionCall(Context &context, const clang::CallExpr *expr);
};

#endif // _ADDACTIONCALL_H
