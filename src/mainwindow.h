#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QIcon>
#include <QMainWindow>
#include <QMessageBox>
#include <QSystemTrayIcon>

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
