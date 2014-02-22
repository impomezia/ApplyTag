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

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>

#include "ApplyTag.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QTextStream out(stdout);
  const QStringList args = app.arguments();

  if (args.size() != 4) {
    out << "Incorrect number of arguments!" << endl;
    out << "Usage: " << QFileInfo(app.applicationFilePath()).fileName() << " <signed file> <output file> <tag file>" << endl;
    return -1;
  }

  QFile tagFile(args.at(3));
  if (!tagFile.exists() || !tagFile.open(QFile::ReadOnly)) {
    out << "File \"" << tagFile.fileName() << "\" not readable!" << endl;
    return ApplyTag::TagFileUnreadable;
  }

  if (tagFile.size() > 65536) {
    out << "File \"" << tagFile.fileName() << "\" too big! Maximum allowed size 65536 bytes." << endl;
    return ApplyTag::TagTooBig;
  }


  ApplyTag tag;
  int result = tag.init(args.at(1), args.at(2), tagFile.readAll());
  if (result) {
    if (result == ApplyTag::BadTagFormat)
      out << "Tag contains invalid data!" << endl;
    else if (result == ApplyTag::SignedExeFileNotExists)
      out << "Signed file \"" << args.at(1) << "\" not exists!" << endl;
    else if (result == ApplyTag::TaggedFileUnwritable)
      out << "Tagged file \"" << args.at(2) << "\" not writable!" << endl;

    return result;
  }

  result = tag.embedTagString();
  if (result == ApplyTag::WriteError)
    out << "Write error, may input file is not signed." << endl;

  return result;
}
