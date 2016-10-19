#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "clang_headers.h"

class Context : public clang::Rewriter {
public:
    const clang::CompilerInstance &_ci;

    Context(const clang::CompilerInstance &ci) :
        clang::Rewriter(ci.getSourceManager(), ci.getLangOpts()),
        _ci(ci) {}

    virtual ~Context();

    void note(const std::string &s);
    void note(clang::SourceLocation loc, const std::string &s);
    void warn(const std::string &s);
    void warn(clang::SourceLocation loc, const std::string &s);

private:
    void processBuffer(const clang::FileID &id, const clang::RewriteBuffer &buffer);
};

#endif // _CONTEXT_H
