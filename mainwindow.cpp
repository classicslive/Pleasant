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

void MainWindow::onFrame(void)
{
  cl_run();
}

int MainWindow::createRetro(const QString& core, const QString& content)
{
  Pleasant *retro = new Pleasant();

  retro->username()->setFromApplication();
  retro->directories()->set(QRetroDirectories::System, "D:\\RetroArch\\system");
  if (!retro->loadCore(core.toStdString().c_str()))
    return 1;
  if (!content.isEmpty() && !retro->loadContent(content.toStdString().c_str()))
    return 2;
  if (!retro->startCore())
    return 3;
  retro->setTitle(retro->core()->system_info.library_name);
  retro->show();

  cl_init(nullptr, 0, content.toStdString().c_str());
  connect(retro, SIGNAL(onFrame(void)), this, SLOT(onFrame(void)));

  return 0;
}

#include <QString>
#include <QDebug>

int MainWindow::createRetroDialog(void)
{
  QFileDialog core_dialog(this);
  QFileDialog content_dialog(this);
  QString core, content = "";

  core_dialog.setFileMode(QFileDialog::ExistingFile);
#ifdef _WIN32
  core_dialog.setNameFilter(tr("libretro cores (*.dll)"));
#else
  core_dialog.setNameFilter(tr("libretro cores (*.so)"));
#endif
  core_dialog.exec();
  if (!core_dialog.selectedFiles().size())
    return 1;
  core = core_dialog.selectedFiles()[0];

  content_dialog.setDirectory(core);
  content_dialog.setNameFilter(tr("Any content file (*)"));
  content_dialog.exec();
  if (!content_dialog.selectedFiles().size())
    content = "";
  else
    content = content_dialog.selectedFiles()[0];

  return createRetro(core, content);
}
