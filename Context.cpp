#include <string>

#include "clang_headers.h"

#include "ConnectCall.h"
#include "Context.h"
#include "SlotCall.h"

using namespace std;
using namespace llvm;
using namespace clang;

void
Context::note(const string &s)
{
    unsigned id = _ci.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Note,
                                                       "qtreslot: %0");
    DiagnosticBuilder builder = _ci.getDiagnostics().Report(id);
    builder << s;
    builder.setForceEmit();
}

void
Context::note(SourceLocation loc, const string &s)
{
    unsigned id = _ci.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Note,
                                                       "qtreslot: %0");
    DiagnosticBuilder builder = _ci.getDiagnostics().Report(loc, id);
    builder << s;
    builder.setForceEmit();
}

void
Context::warn(const string &s)
{
    unsigned id = _ci.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "qtreslot: %0");
    DiagnosticBuilder builder = _ci.getDiagnostics().Report(id);
    builder << s;
    builder.setForceEmit();
}

void
Context::warn(SourceLocation loc, const string &s)
{
    unsigned id = _ci.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "qtreslot: %0");
    DiagnosticBuilder builder = _ci.getDiagnostics().Report(loc, id);
    builder << s;
    builder.setForceEmit();
}

void
Context::processBuffer(const FileID &id, const RewriteBuffer &buffer)
{
    std::error_code error;

    // TODO Avoid writing if output already exists and is up to date.

    const FileEntry *entry = getSourceMgr().getFileEntryForID(id);
    string path((_ci.getFileManager().getCanonicalName(entry->getDir()).str()
                + "/" + entry->getName() + ".qtreslot"));
    note(string("writing ") + path);
    raw_fd_ostream file(StringRef(path), error, llvm::sys::fs::F_None);

    if (error) {
        note(string("unable to open ") + path);
        return;
    }

    buffer.write(file);
    file.close();

    if (error) {
        note(string("error when writing ") + path);
    }
}

Context::~Context()
{
    for (buffer_iterator it = buffer_begin(); it != buffer_end(); it++) {
        processBuffer(it->first, it->second);
    }
}
