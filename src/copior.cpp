#include "copior.h"

Copior::Copior(QStringList pathList, QString rootPath, QObject *parent) : QObject(parent) {
  this->pathList = pathList;
  this->rootPath = rootPath;
}

void Copior::start() {
  for (auto const &a : pathList) {
    recursiveCopy(a);
  }
}

bool Copior::recursiveCopy(QString path) {
  bool isCopied = false;
  QDirIterator it(path, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    // Create folders structure
    if (it.fileInfo().isDir()) {
      isCopied = QDir("").mkpath(rootPath + it.filePath());
    }
    // Copy file
    else {
      // If file doesn't exist or are younger
      if (QFileInfo(rootPath + it.filePath()).exists() && (QFileInfo(it.filePath()).lastModified() < QFileInfo(rootPath + it.filePath()).lastModified())) {
        isCopied = QFile::copy(it.filePath(), rootPath + it.filePath());
      }
      else {
        emit(update(it.filePath() + " copy SKIPPED"));
      }
      // Send status
      if (isCopied) {
        emit(update(it.filePath() + " successfully copied " + rootPath + it.filePath()));
      }
      else {
        emit(update(it.filePath() + " copy FAILED"));
      }
    }
    it.next();
  }
  return isCopied;
}
