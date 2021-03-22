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

  // File model/view local file
  BackupFileModel *backupFileModel = new BackupFileModel;
  ui->ui_backupFileView->setModel(backupFileModel);
  ui->ui_backupFileView->setAcceptDrops(true);
  ui->ui_backupFileView->setDragEnabled(true);
  ui->ui_backupFileView->viewport()->setAcceptDrops(true);
  ui->ui_backupFileView->setDropIndicatorShown(true);

  // Shortcuts
  QShortcut *deleteShort = new QShortcut(QKeySequence(tr("Delete")), ui->ui_backupFileView);

  connect(
      deleteShort, &QShortcut::activated, [this, backupFileModel, deleteShort]() {
        backupFileModel->removeRow(ui->ui_backupFileView->currentIndex().row(), ui->ui_backupFileView->currentIndex().parent());
      });
  connect(ui->ui_backupBut, &QPushButton::clicked, this, &MainWindow::startSaving);

  // Path
  connect(ui->ui_destBut, &QPushButton::clicked, this, &MainWindow::setSaveRoot);

  // Settings
  settings = new QSettings("noname", "B&GInc");
  this->resize(settings->value("mainwindow/size", QSize(400, 400)).toSize());
  this->move(settings->value("mainwindow/pos", QPoint(200, 200)).toPoint());
  ui->ui_destDisp->setText(settings->value("data/rootPath").toString());
  rootSavePath = settings->value("data/rootPath").toString();
  QStringList savePath = settings->value("data/pathList").toStringList();
  for (auto const &a : savePath) {
    backupFileModel->addFolder(QUrl::fromLocalFile(a));
  }
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
  QStringList pathList = qobject_cast<BackupFileModel *>(ui->ui_backupFileView->model())->getPathList();
  QThread *thread = new QThread;
  Copior *copior = new Copior(pathList, this->rootSavePath);
  copior->moveToThread(thread);

  connect(thread, &QThread::started, copior, &Copior::start);
  connect(copior, &Copior::update, ui->ui_log, &QTextEdit::append);
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
}
