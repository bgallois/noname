#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->ui_verticalSplit->setStretchFactor(0, 1);

  // Tray icon
  trayIcon = new QSystemTrayIcon(QIcon(":/assets/icon.svg"));
  QMenu *trayMenu = new QMenu;
  QAction *restore = new QAction(tr("Restore"));
  connect(restore, &QAction::triggered, this, &MainWindow::showNormal);
  trayMenu->addAction(restore);
  QAction *close = new QAction(tr("Close"));
  connect(close, &QAction::triggered, qApp, &QCoreApplication::quit);
  trayMenu->addAction(close);
  connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  // File model/view local file
  QFileSystemModel *localFileModel = new QFileSystemModel;
  localFileModel->setRootPath(QDir::homePath());
  ui->ui_localFileView->setModel(localFileModel);
  ui->ui_localFileView->setCurrentIndex(localFileModel->index(QDir::homePath()));
  localFileModel->setFilter(QDir::AllDirs);

  // File model/view local file
  BackupFileModel *backupFileModel = new BackupFileModel;
  backupFileModel->setHeaderData(0, Qt::Horizontal, "Folder");
  ui->ui_backupFileView->setModel(backupFileModel);
  ui->ui_backupFileView->setAcceptDrops(true);
  ui->ui_backupFileView->setDragEnabled(true);
  ui->ui_backupFileView->viewport()->setAcceptDrops(true);
  ui->ui_backupFileView->setDropIndicatorShown(true);
  ui->ui_backupFileView->expandAll();

  // Shortcuts
  QShortcut *deleteShort = new QShortcut(QKeySequence(tr("Delete")), ui->ui_backupFileView);

  // Toolbar
  QIcon img(":/assets/buttons/remove.png");
  QAction *removeAction = new QAction(img, tr("Remove folder from backup pool"));
  removeAction->setStatusTip(tr("Remove folder from backup pool"));
  connect(removeAction, &QAction::triggered, [this]() {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
    QCoreApplication::postEvent(ui->ui_backupFileView, event);
  });
  ui->toolBar->addAction(removeAction);

  img = QIcon(":/assets/buttons/next.png");
  QAction *addAction = new QAction(img, tr("Add folder to backup pool"));
  removeAction->setStatusTip(tr("Add folder to backup pool"));
  connect(addAction, &QAction::triggered, [this]() {
    ui->ui_backupFileView->model()->dropMimeData(ui->ui_localFileView->model()->mimeData(ui->ui_localFileView->selectionModel()->selectedIndexes()), Qt::CopyAction, -1, -1, ui->ui_backupFileView->model()->index(0, 0));
  });
  ui->toolBar->addAction(addAction);

  connect(deleteShort, &QShortcut::activated, [this, backupFileModel, deleteShort]() {
    // Find root parent and parent row to delete the root folder
    QModelIndex rootParent = ui->ui_backupFileView->currentIndex().parent();
    int row = ui->ui_backupFileView->currentIndex().row();
    while (rootParent.parent() != QModelIndex()) {
      row = rootParent.row();
      rootParent = rootParent.parent();
    }
    backupFileModel->removeFolder(row, rootParent);
  });
  connect(ui->ui_backupBut, &QPushButton::clicked, this, &MainWindow::startSaving);

  // Path
  connect(ui->ui_destBut, &QPushButton::clicked, this, &MainWindow::setSaveRoot);

  // Settings
  settings = new QSettings("QBackup", "B&GInc");
  this->resize(settings->value("mainwindow/size", QSize(400, 400)).toSize());
  this->move(settings->value("mainwindow/pos", QPoint(200, 200)).toPoint());
  ui->ui_destDisp->setText(settings->value("data/rootPath").toString());
  rootSavePath = settings->value("data/rootPath").toString();
  QStringList savePath = settings->value("data/pathList").toStringList();
  for (auto const &a : savePath) {
    backupFileModel->addFolder(QUrl::fromLocalFile(a));
  }

  // Timer
  autoSavingTimer = new QTimer(this);
  connect(ui->ui_time, &QTimeEdit::timeChanged, this, &MainWindow::setTimer);
  connect(autoSavingTimer, &QTimer::timeout, this, &MainWindow::autoSave);
  ui->ui_isAuto->setChecked(settings->value("data/auto").toBool());
  ui->ui_time->setTime(settings->value("data/time").toTime());
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  if (trayIcon->isVisible()) {
    QMessageBox::information(this, tr("Systray"),
                             tr("The program will keep running in the "
                                "system tray. To terminate the program, "
                                "choose <b>Close</b> in the context menu "
                                "of the system tray entry. To restore the "
                                "program choose <b>Restore</b>."));
    hide();
    event->ignore();
  }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger: {
      break;
    }
    case QSystemTrayIcon::DoubleClick: {
      this->setVisible(true);
      break;
    }
    default:;
  }
}

void MainWindow::setSaveRoot() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  ui->ui_destDisp->setText(dir);
  rootSavePath = dir;
}

void MainWindow::startSaving() {
  ui->ui_log->clear();
  trayIcon->showMessage(tr("Starting backup"), tr(""), QSystemTrayIcon::Information, 5000);

  QStringList pathList = qobject_cast<BackupFileModel *>(ui->ui_backupFileView->model())->getPathList();
  QThread *thread = new QThread(this);
  Copior *copior = new Copior(pathList, this->rootSavePath);
  copior->moveToThread(thread);

  connect(thread, &QThread::started, copior, &Copior::start);
  connect(copior, &Copior::update, ui->ui_log, &QTextEdit::append);
  connect(copior, &Copior::finished, this, [this]() {
    trayIcon->showMessage(tr("Backup finished"), tr(""), QSystemTrayIcon::Information, 0);
  });
  connect(copior, &Copior::finished, thread, &QThread::quit);
  connect(copior, &Copior::finished, copior, &Copior::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  thread->start();
}

void MainWindow::saveSettings() {
  settings->setValue("mainwindow/size", this->size());
  settings->setValue("mainwindow/pos", this->pos());
  QStringList pathList = qobject_cast<BackupFileModel *>(ui->ui_backupFileView->model())->getPathList();
  settings->setValue("data/pathList", pathList);
  settings->setValue("data/rootPath", this->rootSavePath);
  settings->setValue("data/auto", ui->ui_isAuto->isChecked());
  settings->setValue("data/time", ui->ui_time->time());
}

void MainWindow::autoSave() {
  if (ui->ui_isAuto->isChecked()) {
    startSaving();
  }
  setTimer(ui->ui_time->time());
}

void MainWindow::setTimer(const QTime &time) {
  int deltaTime = QTime::currentTime().msecsTo(time);
  // If time already passed
  if (deltaTime < 0) {
    deltaTime += 86400000;  // Add 24h
  }
  autoSavingTimer->setSingleShot(true);
  autoSavingTimer->setInterval(deltaTime);
  autoSavingTimer->start();
}
