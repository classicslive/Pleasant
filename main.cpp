#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QRetro.h>

int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  QApplication a(argc, argv);

  MainWindow w;
  w.show();

  return a.exec();
}
