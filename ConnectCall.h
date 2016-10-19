#ifndef _CONNECTCALL_H
#define _CONNECTCALL_H

#include "Context.h"
#include "SlotCall.h"

class ConnectCall : public SlotCall {
public:
    virtual ~ConnectCall() {}

    static bool processCallExpr(Context &context, const clang::CallExpr *expr);

private:
    ConnectCall(Context &context, const clang::CallExpr *expr);
};

#endif // _CONNECTCALL_H
