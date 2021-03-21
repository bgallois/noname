#ifndef BACKUPFILEMODEL_H
#define BACKUPFILEMODEL_H

#include <QAbstractItemModel>
#include <QDebug>
#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMimeData>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>
#include <QWidget>
#include <Qt>

class BackupFileModel : public QStandardItemModel {
  Q_OBJECT
 public:
  BackupFileModel();
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
  Qt::DropActions supportedDropActions() const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QStringList mimeTypes() const override;
  void addFolder(QUrl url);

 public slots:

 private:
  QStandardItem *node;
  QStringList savePath;
  bool setPath(QString path);
 signals:
};

#endif  // BACKUPFILEMODEL_H
