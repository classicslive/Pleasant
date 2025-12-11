#include "mainwindow.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>

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

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  auto history = settings.value("history").toJsonArray();
  auto menu_history = menu_file->addMenu(tr("History"));
  for (const auto history_entry : history)
  {
    if (history_entry.isObject())
    {
      const QJsonObject& entry = history_entry.toObject();
      QString core = entry.value("core_path").toString();
      QString content = entry.value("content_path").toString();
      QString core_name = entry.value("core_name").toString();
      QString item_text = tr("%1 (%2)").arg(QFileInfo(content).baseName(), core_name);

      QAction* history_action = new QAction(item_text, menu_history);
      menu_history->addAction(history_action);
      connect(history_action, &QAction::triggered, [=]() { createRetro(core, content); });
    }
  }

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

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  auto classicslive = settings.value("classicslive", true).toBool();
  auto history = settings.value("history").toJsonArray();

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

  QJsonObject history_entry;
  history_entry["core_path"] = core;
  history_entry["content_path"] = content;
  history_entry["core_name"] = retro->core()->system_info.library_name;
  bool new_content = true;
  for (const auto entry : history)
  {
    if (entry.toObject().value("content_path").toString() == content)
    {
      new_content = false;
      break;
    }
  }
  if (new_content)
  {
    history.append(history_entry);
    settings.setValue("history", history);
    settings.sync();
  }

  if (classicslive)
  {
    cl_game_identifier_t identifier;

    memset(&identifier, 0, sizeof(identifier));
    snprintf(identifier.filename, sizeof(identifier.filename), "%s", content.toStdString().c_str());
    identifier.type = CL_GAMEIDENTIFIER_FILE_HASH;
    cl_login_and_start(identifier);
    connect(retro, SIGNAL(onFrame(void)), this, SLOT(onFrame(void)));
  }

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
