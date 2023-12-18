#include "mainwindow.h"

#include <QFileDialog>
#include <QMenuBar>

extern "C"
{
  #include <cl_main.h>
};

#include <Pleasant.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  auto action_load = new QAction(tr("&Load core and content"), this);
  connect(action_load, SIGNAL(triggered()), this, SLOT(createRetroDialog()));
  auto menu_file = menuBar()->addMenu(tr("&File"));
  auto menu_history = menu_file->addMenu(tr("History"));

  menu_file->addAction(action_load);
}

MainWindow::~MainWindow()
{
}

int MainWindow::createRetro(const QString& core, const QString& content)
{
  Pleasant *retro = new Pleasant();

  retro->username()->setFromApplication();
  retro->directories()->set(QRetroDirectories::System, "D:\\RetroArch\\system");
  if (!retro->loadCore(core.toStdString().c_str()))
    return 1;
  if (!retro->loadContent(content.toStdString().c_str()))
    return 2;
  if (!retro->startCore())
    return 3;
  retro->setTitle(retro->username()->get());
  retro->show();

  cl_init(nullptr, 0, content.toStdString().c_str());

  return 0;
}

#include <QString>
#include <QDebug>

int MainWindow::createRetroDialog(void)
{
  QFileDialog dialog(this);
  QString core, content = "";

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter(tr("libretro cores (*.dll)"));
  dialog.exec();
  if (!dialog.selectedFiles().size())
    return 1;
  core = dialog.selectedFiles()[0];

  dialog.setDirectory(core);
  dialog.setNameFilter(tr("Any content file (*)"));
  dialog.exec();
  if (!dialog.selectedFiles().size())
    return 1;
  content = dialog.selectedFiles()[0];

  return createRetro(core, content);
}
