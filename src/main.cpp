#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setApplicationName("QBackup");
  a.setOrganizationName("B&GIn");

  MainWindow w;
  w.setWindowIcon(QIcon(":/assets/icon.png"));
  QFontDatabase::addApplicationFont(":/assets/Font.ttf");
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");
  QFile file(":/assets/theme.qss");
  file.open(QFile::ReadOnly | QFile::Text);
  QTextStream stream(&file);
  qApp->setStyleSheet(stream.readAll());
  w.show();
  return a.exec();
}
