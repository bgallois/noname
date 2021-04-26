#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QIcon>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMainWindow>
#include <QMessageBox>
#include <QModelIndex>
#include <QSettings>
#include <QShortcut>
#include <QString>
#include <QSystemTrayIcon>
#include <QThread>
#include <QTime>
#include <QTimer>
#include "backupfilemodel.h"
#include "copior.h"

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
  QString rootSavePath;
  QSettings *settings;
  QTimer *autoSavingTimer;
  void autoSave();

 private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void setSaveRoot();
  void startSaving();
  void saveSettings();
  void setTimer(const QTime &time);
};
#endif  // MAINWINDOW_H
