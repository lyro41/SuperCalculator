#include "mainwindow.h"
#include "bignumber.h"

#include <QApplication>


int main(int argc, char *argv[]) {

  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
