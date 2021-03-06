#include "backupfilemodel.h"

BackupFileModel::BackupFileModel() {
  node = this->invisibleRootItem();
  QStandardItem *item = new QStandardItem("Backup_Folder");
  item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsDragEnabled & ~Qt::ItemIsEnabled);
  node->appendRow(item);
  node = item;
}

Qt::DropActions BackupFileModel::supportedDropActions() const {
  return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::ItemFlags BackupFileModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
  if (index.isValid())
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
  else
    return Qt::ItemIsDropEnabled | defaultFlags;
}

bool BackupFileModel::dropMimeData(const QMimeData *data,
                                   Qt::DropAction action, int row, int column, const QModelIndex &parent) {
  bool success = true;
  QList<QUrl> urls = data->urls();
  for (auto &a : urls) {
    addFolder(a);
  }
  return success;
}

QStringList BackupFileModel::mimeTypes() const {
  return QStringList(QLatin1String("text/uri-list"));
}

void BackupFileModel::addFolder(QUrl url) {
  QString path = url.toLocalFile();
  if (setPath(path)) {
    QStringList splitPath = path.split("/", QString::SkipEmptyParts);
    QFileIconProvider provider;
    QStandardItem *lastNode = node;
    for (auto const &i : splitPath) {
      QStandardItem *item = new QStandardItem(provider.icon(QFileIconProvider::Folder), i);
      lastNode->appendRow(item);
      lastNode = item;
    }
  }
}

void BackupFileModel::removeFolder(int row, const QModelIndex &parent) {
  removeRow(row, parent);
  savePath.removeAt(row);
}

bool BackupFileModel::setPath(QString path) {
  QString absolutePath = QDir("/" + path).absolutePath();
  for (int i = 0; i < savePath.size(); i++) {
    if (absolutePath.contains(savePath[i])) {
      return false;
    }
    else if (savePath[i].contains(absolutePath)) {
      removeFolder(i, index(0, 0));
    }
  }
  savePath.append(absolutePath);
  return true;
}

QStringList BackupFileModel::getPathList() {
  return savePath;
}
