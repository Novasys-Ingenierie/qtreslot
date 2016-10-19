#ifndef _DISCONNECTCALL_H
#define _DISCONNECTCALL_H

#include "Context.h"
#include "SlotCall.h"

class DisconnectCall : public SlotCall {
public:
    virtual ~DisconnectCall() {}

    static bool processCallExpr(Context &context, const clang::CallExpr *expr);

private:
    DisconnectCall(Context &context, const clang::CallExpr *expr);
};

#endif // _DISCONNECTCALL_H
