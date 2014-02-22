// Copyright 2006-2009 Google Inc.
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
//
// Applies a tag to a signed file.

#include <QtEndian>

#include "ApplyTag.h"

const char kMagicBytes[] = "Gact";
const quint32 kPEHeaderOffset = 60;


int ApplyTag::embedTagString()
{
  createBufferToWrite();

  m_taggedFile.seek(m_taggedFile.size());
  m_taggedFile.write(m_tagBuffer);

  if (!applyTagToBuffer()) {
    m_taggedFile.close();
    return WriteError;
  }

  m_taggedFile.close();
  return NoError;
}


int ApplyTag::init(const QString &signedExeFile, const QString &taggedFile, const QByteArray &tag)
{
  if (tag.size() != (int) qstrlen(tag.constData()))
    return BadTagFormat;

  if (tag.size() > 65536)
    return TagTooBig;

  m_tag = tag;

  if (!QFile::exists(signedExeFile))
    return SignedExeFileNotExists;

  if (QFile::exists(taggedFile) && !QFile::remove(taggedFile))
    return TaggedFileUnwritable;

  if (!QFile::copy(signedExeFile, taggedFile))
    return TaggedFileUnwritable;

  m_taggedFile.setFileName(taggedFile);
  if (!m_taggedFile.open(QFile::ReadWrite))
    return TaggedFileUnwritable;

  return NoError;
}


bool ApplyTag::applyTagToBuffer() {
  quint32 peHeader             = getUint32(kPEHeaderOffset);
  quint32 certDirAddressOffset = 152;

  // Read certificate directory info.
  quint32 certDirOffset = getUint32(peHeader + certDirAddressOffset);
  if (certDirOffset == 0)
    return false;

  quint32 certDirSize = getUint32(peHeader + certDirAddressOffset + 4);

  // Increase the size of certificate directory.
  int newCertSize = certDirSize + m_tagBuffer.size();

  putUint32(newCertSize, peHeader + certDirAddressOffset + 4);

  // Increase the certificate struct size.
  putUint32(newCertSize, certDirOffset);

  return true;
}


quint32 ApplyTag::getUint32(qint64 offset)
{
  quint32 out = 0;
  m_taggedFile.seek(offset);
  m_taggedFile.read((char *) &out, 4);
  return out;
}


void ApplyTag::createBufferToWrite()
{
  // Build the tag buffer.
  // The format of the tag buffer is:
  // 000000-000003: 4-byte magic (big-endian)
  // 000004-000005: unsigned 16-bit int string length (big-endian)
  // 000006-??????: ASCII string
  qint16 size = m_tag.size();
  int unpaddedSize = size + 6;
  // The tag buffer should be padded to multiples of 8, otherwise it will
  // break the signature of the executable file.
  int paddedSize = (unpaddedSize + 15) & (-8);

  m_tagBuffer.clear();
  m_tagBuffer.reserve(paddedSize);
  m_tagBuffer.append(kMagicBytes);

  size = qToBigEndian(size);
  m_tagBuffer.append((char *) &size, 2);
  m_tagBuffer.append(m_tag);
  m_tagBuffer.append(QByteArray().fill('\0', paddedSize - unpaddedSize));
}


void ApplyTag::putUint32(quint32 i, qint64 offset)
{
  m_taggedFile.seek(offset);
  m_taggedFile.write((char *) &i, 4);
}
