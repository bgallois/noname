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
  QDirIterator it(path, QDir::AllEntries | QDir::Hidden | QDir::System, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    // Create folders structure
    if (it.fileInfo().isDir()) {
      isCopied = QDir("").mkpath(rootPath + it.filePath());
    }
    // Copy file
    else {
      // If file doesn't exist or are younger
      if (!QFileInfo(rootPath + it.filePath()).exists() || (QFileInfo(it.filePath()).lastModified() > QFileInfo(rootPath + it.filePath()).lastModified())) {
        QFile::remove(rootPath + it.filePath());
        isCopied = QFile::copy(it.filePath(), rootPath + it.filePath());
        if (isCopied) {
          emit(update("<p style='color:green';>" + it.filePath() + " copied <b>SUCCESSFULLY</b> to " + rootPath + it.filePath() + "</p>"));
        }
        else {
          emit(update("<p style='color:red';>" + it.filePath() + " copy <b>FAILED</b></p>"));
        }
      }
      else {
        emit(update("<p style='color:blue';>" + it.filePath() + " copy <b>SKIPPED</b></p>"));
      }
    }
    it.next();
  }
  return isCopied;
}
