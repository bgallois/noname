#ifndef COPIOR_H
#define COPIOR_H

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QVariant>

class Copior : public QObject {
  Q_OBJECT
 public:
  Copior() = default;
  explicit Copior(QStringList pathList, QString rootPath, QObject *parent = nullptr);
  Copior(const Copior &T) = delete;
  Copior &operator=(const Copior &T) = delete;

 public slots:
  void start();

 signals:
  void update(QString message);
  void finished();
  void progressChanged(int value);

 private:
  QStringList pathList;
  QString rootPath;
  bool recursiveCopy(QString path);
};

#endif  // COPIOR_H
