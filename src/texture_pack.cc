/* Copyright 2012 Brian Ellis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "texture_pack.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>

#include <QJson/Parser>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "macros.h"

static const char* kPossibleAppDataDirs[] = {"Roaming", "Local", "LocalLow"};

// Static.
TexturePack* TexturePack::createDefaultTexturePack() {
  return new TexturePack("Default texture pack", QFileInfo(minecraftDirectory(), "bin/minecraft.jar"));
}

// Static.
QDir TexturePack::minecraftDirectory() {
  QDir dir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
  dir.cdUp();
  // On Mac or Linux, this is probably the right directory, so look for .minecraft or minecraft
  // as a subfolder.
  if (dir.cd(".minecraft") || dir.cd("minecraft")) {
    return dir;
  } else {
    // Oh boy. This is Windows, isn't it? And you gave us the wrong AppData directory, didn't you?
    // Yes, that's right, don't try to deny it, I see that guilty look in your eyes. Okay, fine,
    // we'll go look at all the others too.
    dir.cdUp();
    for (int i = 0; i < arraysize(kPossibleAppDataDirs); ++i) {
      if (dir.cd(kPossibleAppDataDirs[i])) {
        if (dir.cd(".minecraft") || dir.cd("minecraft")) {
          return dir;
        }
        dir.cdUp();
      }
    }
    // I give up, we can't find it. :(
    qWarning() << "Can't find the Minecraft directory!";
    return QDir();
  }
}


TexturePack::TexturePack(const QString& name,
                         const QFileInfo& archive,
                         const TexturePack* minecraft_jar,
                         QObject* parent)
    : QObject(parent), name_(name), minecraft_jar_(minecraft_jar) {
  QuaZip zip(archive.absoluteFilePath());
  zip.open(QuaZip::mdUnzip);
  QuaZipFile file(&zip);
  qDebug() << "Loading minecraft.jar...";
  for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
    if (file.getActualFileName().endsWith("terrain.png", Qt::CaseInsensitive)) {
      file.open(QIODevice::ReadOnly);
      QPixmap pmap;
      pmap.loadFromData(file.readAll());
      tile_sheets_.insert(file.getActualFileName(), pmap);
      file.close();
    } else if (file.getActualFileName() == "pack.txt") {
      // Read pack.txt to get a better name for the pack than the zipfile name.
      file.open(QIODevice::ReadOnly);
      QByteArray bytes = file.readLine();
      name_ = QString::fromUtf8(bytes.constData()).trimmed();
      file.close();
    }
  }
  zip.close();
  qDebug() << "All done.";
}

QString TexturePack::name() const {
  return name_;
}

QPixmap TexturePack::tileSheetNamed(const QString& name) const {
  if (tile_sheets_.contains(name)) {
    return tile_sheets_.value(name);
  }
  if (minecraft_jar_) {
    return minecraft_jar_->tileSheetNamed(name);
  } else {
    qWarning() << "Unable to find tilesheet" << name << "in this or default texture pack!";
    return QPixmap();
  }
}
