#ifndef _SINGLESHOTCALL_H
#define _SINGLESHOTCALL_H

#include "Context.h"
#include "SlotCall.h"

class SingleShotCall : public SlotCall {
public:
    virtual ~SingleShotCall() {}

    static bool processCallExpr(Context &context, const clang::CallExpr *expr);

private:
    SingleShotCall(Context &context, const clang::CallExpr *expr);
};

#endif // _SINGLESHOTCALL_H
