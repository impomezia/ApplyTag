// Copyright 2007-2009 Google Inc.
// Copyright 2013-2014 Alexander Sedov <support@schat.me>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ========================================================================

#ifndef APPLYTAG_H_
#define APPLYTAG_H_

#include <QFile>

class ApplyTag
{
public:
  enum ErrorCodes {
    NoError,
    SignedExeFileNotExists,
    TaggedFileUnwritable,
    TagFileUnreadable,
    BadTagFormat,
    TagTooBig,
    WriteError
  };

  inline ApplyTag() {}
  int embedTagString();
  int init(const QString &signedExeFile, const QString &taggedFile, const QByteArray &tag);

private:
  bool applyTagToBuffer();
  quint32 getUint32(qint64 offset);
  void createBufferToWrite();
  void putUint32(quint32 i, qint64 offset);

  QByteArray m_tag;        ///< The byte array to be tagged into the binary.
  QFile m_taggedFile;      ///< The output file.
  QByteArray m_tagBuffer;  ///< Internal buffer to hold the appended byte array.
};

#endif // APPLYTAG_H_
