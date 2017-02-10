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
