#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QDir>
#include <QFileSystemModel>
#include <QIcon>
#include <QKeySequence>
#include <QMainWindow>
#include <QMessageBox>
#include <QShortcut>
#include <QString>
#include <QSystemTrayIcon>
#include "backupfilemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 protected:
  void closeEvent(QCloseEvent *event);

 private:
  Ui::MainWindow *ui;
  QSystemTrayIcon *trayIcon;

 private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
};
#endif  // MAINWINDOW_H
