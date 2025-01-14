//===-- SBFile.cpp ------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/API/SBFile.h"
#include "SBReproducerPrivate.h"
#include "lldb/API/SBError.h"
#include "lldb/Host/File.h"

using namespace lldb;
using namespace lldb_private;

SBFile::~SBFile() {}

SBFile::SBFile(FileSP file_sp) : m_opaque_sp(file_sp) {
  LLDB_RECORD_DUMMY(void, SBfile, SBFile, (FileSP), file_sp);
}

SBFile::SBFile() { LLDB_RECORD_CONSTRUCTOR_NO_ARGS(SBFile); }

SBFile::SBFile(FILE *file, bool transfer_ownership) {
  LLDB_RECORD_DUMMY(void, SBFile, (FILE *, bool), file, transfer_ownership);
  m_opaque_sp = std::make_shared<NativeFile>(file, transfer_ownership);
}

SBFile::SBFile(int fd, const char *mode, bool transfer_owndership) {
  LLDB_RECORD_DUMMY(void, SBFile, (int, const char *, bool), fd, mode,
                    transfer_owndership);
  auto options = File::GetOptionsFromMode(mode);
  if (!options) {
    llvm::consumeError(options.takeError());
    return;
  }
  m_opaque_sp =
      std::make_shared<NativeFile>(fd, options.get(), transfer_owndership);
}

SBError SBFile::Read(uint8_t *buf, size_t num_bytes, size_t *bytes_read) {
  LLDB_RECORD_DUMMY(lldb::SBError, SBFile, Read, (uint8_t *, size_t, size_t *),
                    buf, num_bytes, bytes_read);
  SBError error;
  if (!m_opaque_sp) {
    error.SetErrorString("invalid SBFile");
    *bytes_read = 0;
  } else {
    Status status = m_opaque_sp->Read(buf, num_bytes);
    error.SetError(status);
    *bytes_read = num_bytes;
  }
  return LLDB_RECORD_RESULT(error);
}

SBError SBFile::Write(const uint8_t *buf, size_t num_bytes,
                      size_t *bytes_written) {
  LLDB_RECORD_DUMMY(lldb::SBError, SBFile, Write,
                    (const uint8_t *, size_t, size_t *), buf, num_bytes,
                    bytes_written);
  SBError error;
  if (!m_opaque_sp) {
    error.SetErrorString("invalid SBFile");
    *bytes_written = 0;
  } else {
    Status status = m_opaque_sp->Write(buf, num_bytes);
    error.SetError(status);
    *bytes_written = num_bytes;
  }
  return LLDB_RECORD_RESULT(error);
}

SBError SBFile::Flush() {
  LLDB_RECORD_METHOD_NO_ARGS(lldb::SBError, SBFile, Flush);
  SBError error;
  if (!m_opaque_sp) {
    error.SetErrorString("invalid SBFile");
  } else {
    Status status = m_opaque_sp->Flush();
    error.SetError(status);
  }
  return LLDB_RECORD_RESULT(error);
}

bool SBFile::IsValid() const {
  LLDB_RECORD_METHOD_CONST_NO_ARGS(bool, SBFile, IsValid);
  return m_opaque_sp && m_opaque_sp->IsValid();
}

SBError SBFile::Close() {
  LLDB_RECORD_METHOD_NO_ARGS(lldb::SBError, SBFile, Close);
  SBError error;
  if (m_opaque_sp) {
    Status status = m_opaque_sp->Close();
    error.SetError(status);
  }
  return LLDB_RECORD_RESULT(error);
}

SBFile::operator bool() const {
  LLDB_RECORD_METHOD_CONST_NO_ARGS(bool, SBFile, operator bool);
  return LLDB_RECORD_RESULT(IsValid());
}

bool SBFile::operator!() const {
  LLDB_RECORD_METHOD_CONST_NO_ARGS(bool, SBFile, operator!);
  return LLDB_RECORD_RESULT(!IsValid());
}

namespace lldb_private {
namespace repro {

template <> void RegisterMethods<SBFile>(Registry &R) {

  LLDB_REGISTER_METHOD(lldb::SBError, SBFile, Flush, ());
  LLDB_REGISTER_METHOD_CONST(bool, SBFile, IsValid, ());
  LLDB_REGISTER_METHOD_CONST(bool, SBFile, operator bool,());
  LLDB_REGISTER_METHOD_CONST(bool, SBFile, operator!,());
  LLDB_REGISTER_METHOD(lldb::SBError, SBFile, Close, ());
}
} // namespace repro
} // namespace lldb_private
