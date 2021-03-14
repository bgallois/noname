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
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
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
