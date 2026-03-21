#include "mainwindow.h"

#include <functional>

#include <QCheckBox>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPainter>
#include <QPixmap>
#include <QScrollArea>
#include <QSettings>
#include <QUrl>
#include <QVBoxLayout>

extern "C"
{
  #include <cl_main.h>
  #include <cl_network.h>
};

#include <Pleasant.h>

class HistoryEntry : public QFrame
{
public:
  HistoryEntry(const QString& core, const QString& content, const QString& core_name,
               const QString& icon_filename, const QString& game_title, int game_id,
               std::function<void()> on_click, QWidget* parent = nullptr)
    : QFrame(parent), m_on_click(on_click), m_Content(content), m_GameId(game_id)
  {
    setFrameShape(QFrame::NoFrame);
    setAutoFillBackground(true);
    setCursor(Qt::PointingHandCursor);

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    auto icon_label = new QLabel(this);
    icon_label->setFixedSize(64, 64);
    icon_label->setAlignment(Qt::AlignCenter);
    icon_label->setStyleSheet("background-color: palette(mid); border-radius: 4px;");
    if (!icon_filename.isEmpty())
    {
      QPixmap pix(QDir::currentPath() + "/classicslive/images/game/icon/" + icon_filename + ".png");
      if (!pix.isNull())
        icon_label->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    layout->addWidget(icon_label);

    QString display_name = game_title.isEmpty() ? QFileInfo(content).baseName() : game_title;
    auto name_label = new QLabel(display_name, this);
    QFont name_font = name_label->font();
    name_font.setBold(true);
    name_font.setPointSize(11);
    name_label->setFont(name_font);

    auto core_label = new QLabel(core_name, this);
    QPalette p = core_label->palette();
    p.setColor(QPalette::WindowText, QColor(128, 128, 128));
    core_label->setPalette(p);

    auto text_layout = new QVBoxLayout();
    text_layout->setContentsMargins(0, 0, 0, 0);
    text_layout->setSpacing(2);
    text_layout->addWidget(name_label);
    text_layout->addWidget(core_label);

    text_layout->addStretch();
    layout->addLayout(text_layout, 1);
  }

protected:
  void enterEvent(QEvent* e) override
  {
    QPalette p = palette();
    p.setColor(QPalette::Window, p.alternateBase().color());
    setPalette(p);
    QFrame::enterEvent(e);
  }

  void leaveEvent(QEvent* e) override
  {
    setPalette(QPalette());
    QFrame::leaveEvent(e);
  }

  void paintEvent(QPaintEvent* e) override
  {
    QFrame::paintEvent(e);
    QPainter p(this);
    p.setPen(palette().mid().color());
    p.drawLine(0, height() - 1, width() - 1, height() - 1);
  }

  void mousePressEvent(QMouseEvent* event) override
  {
    QFrame::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
      m_on_click();
  }

  void contextMenuEvent(QContextMenuEvent *e) override
  {
    QMenu menu(this);

    menu.addAction(tr("Start Game"), [this]() { m_on_click(); });
    menu.addSeparator();
    if (m_GameId != 0)
    {
      menu.addAction(tr("Visit Game Page"), [this]() {
        QDesktopServices::openUrl(
          QUrl(QString(CL_URL_SITE) + "/game/" + QString::number(m_GameId)));
      });
    }
    menu.addAction(tr("Remove from History"), [this]() {
      QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
      QJsonArray history = settings.value("history").toJsonArray();
      for (int i = 0; i < history.size(); i++)
      {
        if (history[i].toObject().value("content_path").toString() == m_Content)
        {
          history.removeAt(i);
          break;
        }
      }
      settings.setValue("history", history);
      settings.sync();
      deleteLater();
    });

    menu.exec(e->globalPos());
  }

private:
  std::function<void()> m_on_click;
  QString m_Content;
  int m_GameId = 0;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  auto action_load = new QAction(tr("&Load core and content"), this);
  connect(action_load, SIGNAL(triggered()), this, SLOT(createRetroDialog()));
  auto menu_file = menuBar()->addMenu(tr("&File"));

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  m_LastCoreDir    = settings.value("lastCoreDir",    QDir::homePath()).toString();
  m_LastContentDir = settings.value("lastContentDir", QDir::homePath()).toString();
  auto history = settings.value("history").toJsonArray();
  auto cores = settings.value("cores").toJsonArray();
  auto menu_history = menu_file->addMenu(tr("History"));

  auto menu_recent_cores = menu_history->addMenu(tr("Recent Cores"));
  for (const auto cores_entry : cores)
  {
    if (cores_entry.isObject())
    {
      const QJsonObject& entry = cores_entry.toObject();
      QString core = entry.value("core_path").toString();
      QString core_name = entry.value("core_name").toString();

      QAction* core_action = new QAction(core_name, menu_recent_cores);
      menu_recent_cores->addAction(core_action);
      connect(core_action, &QAction::triggered, [=]() { createRetroContentDialog(core); });
    }
  }

  menu_history->addSeparator();

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

  /* ── Classics Live header bar ───────────────────────────────── */
  auto *clsBar = new QFrame();
  clsBar->setFrameShape(QFrame::StyledPanel);
  clsBar->setFrameShadow(QFrame::Raised);
  auto *clsLayout = new QHBoxLayout(clsBar);
  clsLayout->setContentsMargins(10, 6, 10, 6);
  clsLayout->setSpacing(8);

  m_ClsEnabled = new QCheckBox(tr("Classics Live"));
  {
    QFont f = m_ClsEnabled->font();
    f.setBold(true);
    m_ClsEnabled->setFont(f);
  }
  m_ClsEnabled->setChecked(settings.value("classicslive", false).toBool());

  auto *clsUserLabel = new QLabel(tr("Username:"));
  m_ClsUsername = new QLineEdit();
  m_ClsUsername->setPlaceholderText(tr("Username"));
  m_ClsUsername->setText(settings.value("clsUsername", "").toString());
  m_ClsUsername->setFixedWidth(130);

  auto *clsPassLabel = new QLabel(tr("Password:"));
  m_ClsPassword = new QLineEdit();
  m_ClsPassword->setPlaceholderText(tr("Password"));
  m_ClsPassword->setEchoMode(QLineEdit::Password);
  m_ClsPassword->setText(settings.value("clsPassword", "").toString());
  m_ClsPassword->setFixedWidth(110);

  clsLayout->addWidget(m_ClsEnabled);
  clsLayout->addStretch();
  clsLayout->addWidget(clsUserLabel);
  clsLayout->addWidget(m_ClsUsername);
  clsLayout->addWidget(clsPassLabel);
  clsLayout->addWidget(m_ClsPassword);

  auto saveClsSettings = [this]() {
    bool on = m_ClsEnabled->isChecked();
    m_ClsUsername->setEnabled(on);
    m_ClsPassword->setEnabled(on);
    QSettings s(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
    s.setValue("classicslive", on);
    s.setValue("clsUsername", m_ClsUsername->text());
    s.setValue("clsPassword", m_ClsPassword->text());
    s.sync();
  };

  {
    bool on = m_ClsEnabled->isChecked();
    m_ClsUsername->setEnabled(on);
    m_ClsPassword->setEnabled(on);
  }

  connect(m_ClsEnabled, &QCheckBox::stateChanged,
          [saveClsSettings](int) { saveClsSettings(); });
  connect(m_ClsUsername, &QLineEdit::editingFinished, saveClsSettings);
  connect(m_ClsPassword, &QLineEdit::editingFinished, saveClsSettings);

  /* ── History scroll area ─────────────────────────────────────── */
  auto scroll_area = new QScrollArea();
  scroll_area->setWidgetResizable(true);
  scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll_area->setFrameShape(QFrame::NoFrame);

  auto container = new QWidget(scroll_area);
  auto container_layout = new QVBoxLayout(container);
  container_layout->setContentsMargins(0, 0, 0, 0);
  container_layout->setSpacing(0);

  for (const auto history_entry : history)
  {
    if (history_entry.isObject())
    {
      const QJsonObject& entry = history_entry.toObject();
      QString core = entry.value("core_path").toString();
      QString content = entry.value("content_path").toString();
      QString core_name = entry.value("core_name").toString();
      QString icon_filename = entry.value("icon_filename").toString();
      QString game_title = entry.value("game_title").toString();
      int game_id = entry.value("game_id").toInt();

      auto row = new HistoryEntry(core, content, core_name, icon_filename, game_title, game_id,
                                  [=]() { createRetro(core, content); }, container);
      container_layout->addWidget(row);
    }
  }

  container_layout->addStretch();
  scroll_area->setWidget(container);

  auto *central = new QWidget(this);
  auto *centralLayout = new QVBoxLayout(central);
  centralLayout->setContentsMargins(0, 0, 0, 0);
  centralLayout->setSpacing(0);
  centralLayout->addWidget(clsBar);
  auto *clsSep = new QFrame();
  clsSep->setFrameShape(QFrame::HLine);
  clsSep->setFrameShadow(QFrame::Sunken);
  centralLayout->addWidget(clsSep);
  centralLayout->addWidget(scroll_area, 1);

  setCentralWidget(central);
  setMinimumSize(480, 320);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onFrame(void)
{
  cl_run();

  if (!m_IconDownloadTriggered && session.state == CL_SESSION_STARTED)
  {
    m_IconDownloadTriggered = true;

    QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
    QJsonArray history = settings.value("history").toJsonArray();
    for (int i = 0; i < history.size(); i++)
    {
      QJsonObject entry = history[i].toObject();
      if (entry.value("content_path").toString() == m_CurrentContent)
      {
        entry["game_title"] = QString(session.game_title);
        entry["game_id"] = static_cast<int>(session.game_id);
        history[i] = entry;
        break;
      }
    }
    settings.setValue("history", history);
    settings.sync();

    if (session.icon_url[0])
      downloadGameIcon(QString(session.icon_url), m_CurrentContent);
  }
}

int MainWindow::createRetro(const QString& core, const QString& content)
{
  Pleasant *retro = new Pleasant();

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  auto classicslive = settings.value("classicslive", false).toBool();
  auto history = settings.value("history").toJsonArray();

  retro->username()->setFromApplication();
  //retro->directories()->set(QRetroDirectories::System, "D:\\RetroArch\\system");
  if (!retro->loadCore(core.toStdString().c_str()))
    return 1;
  if (!content.isEmpty() && !retro->loadContent(content.toStdString().c_str()))
    return 2;
  if (!retro->startCore())
    return 3;
  retro->setTitle(retro->core()->system_info.library_name);
  retro->show();

  const QString core_name = retro->core()->system_info.library_name;

  QJsonObject history_entry;
  history_entry["core_path"] = core;
  history_entry["content_path"] = content;
  history_entry["core_name"] = core_name;
  history_entry["icon_filename"] = "";
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
  }

  auto cores = settings.value("cores").toJsonArray();
  bool new_core = true;
  for (const auto entry : cores)
  {
    if (entry.toObject().value("core_path").toString() == core)
    {
      new_core = false;
      break;
    }
  }
  if (new_core)
  {
    QJsonObject core_entry;
    core_entry["core_path"] = core;
    core_entry["core_name"] = core_name;
    cores.append(core_entry);
    settings.setValue("cores", cores);
  }

  settings.sync();

  if (classicslive)
  {
    cl_game_identifier_t identifier;

    m_CurrentContent = content;
    m_IconDownloadTriggered = false;

    cls_abi_register();
    memset(&identifier, 0, sizeof(identifier));
    snprintf(identifier.filename, sizeof(identifier.filename), "%s", content.toStdString().c_str());
    identifier.type = CL_GAMEIDENTIFIER_FILE_HASH;
    cl_login_and_start(identifier);
    connect(retro, SIGNAL(onFrame()), this, SLOT(onFrame()));
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
  core_dialog.setDirectory(m_LastCoreDir);
#ifdef _WIN32
  core_dialog.setNameFilter(tr("libretro cores (*.dll)"));
#else
  core_dialog.setNameFilter(tr("libretro cores (*.so)"));
#endif
  core_dialog.exec();
  if (!core_dialog.selectedFiles().size())
    return 1;
  core = core_dialog.selectedFiles()[0];
  m_LastCoreDir = QFileInfo(core).absolutePath();

  content_dialog.setDirectory(m_LastContentDir);
  content_dialog.setNameFilter(tr("Any content file (*)"));
  content_dialog.exec();
  if (!content_dialog.selectedFiles().size())
    content = "";
  else
  {
    content = content_dialog.selectedFiles()[0];
    m_LastContentDir = QFileInfo(content).absolutePath();
  }

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  settings.setValue("lastCoreDir",    m_LastCoreDir);
  settings.setValue("lastContentDir", m_LastContentDir);

  return createRetro(core, content);
}

int MainWindow::createRetroContentDialog(const QString& core)
{
  QFileDialog content_dialog(this);
  QString content = "";

  content_dialog.setDirectory(m_LastContentDir);
  content_dialog.setNameFilter(tr("Any content file (*)"));
  content_dialog.exec();
  if (!content_dialog.selectedFiles().size())
    return 1;
  content = content_dialog.selectedFiles()[0];
  m_LastContentDir = QFileInfo(content).absolutePath();

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  settings.setValue("lastContentDir", m_LastContentDir);

  return createRetro(core, content);
}

void MainWindow::downloadGameIcon(const QString &url, const QString &content)
{
  QString normalized = QString(url).replace('\\', '/');
  QUrl final_url = QString(CL_URL_SITE) + "/storage/" + normalized;
  QString basename = QFileInfo(normalized).baseName();

  if (basename.isEmpty())
    return;
  QDir(QDir::currentPath() + "/classicslive/images/game/icon").mkpath(".");

  QNetworkRequest request(final_url);
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                        QNetworkRequest::NoLessSafeRedirectPolicy);
  QNetworkReply *reply = m_IconDownloader.get(request);
  connect(reply, &QNetworkReply::finished, [this, reply, basename, content]() {
    onIconDownloaded(reply, basename, content);
  });
}

void MainWindow::onIconDownloaded(QNetworkReply *reply, const QString &basename,
                                  const QString &content)
{
  auto error = reply->error();

  reply->deleteLater();
  if (error != QNetworkReply::NoError)
    return;

  QString path = QDir::currentPath() + "/classicslive/images/game/icon/" + basename + ".png";
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly))
    return;
  file.write(reply->readAll());
  file.close();

  QSettings settings(QDir::currentPath() + "/history.ini", QSettings::IniFormat);
  QJsonArray history = settings.value("history").toJsonArray();
  for (int i = 0; i < history.size(); i++)
  {
    QJsonObject entry = history[i].toObject();
    if (entry.value("content_path").toString() == content)
    {
      entry["icon_filename"] = basename;
      history[i] = entry;
      break;
    }
  }
  settings.setValue("history", history);
  settings.sync();
}
