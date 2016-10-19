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
