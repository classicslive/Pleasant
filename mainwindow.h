#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

class QCheckBox;
class QLineEdit;
class QNetworkReply;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  int createRetro(const QString& core, const QString& content);
  int createRetroDialog(void);
  int createRetroContentDialog(const QString& core);
  void onFrame(void);

private:
  void downloadGameIcon(const QString &url, const QString &content);
  void onIconDownloaded(QNetworkReply *reply, const QString &basename, const QString &content);

  QNetworkAccessManager m_IconDownloader;
  QString m_CurrentContent;
  bool m_IconDownloadTriggered = false;

  QString m_LastCoreDir;
  QString m_LastContentDir;

  QCheckBox *m_ClsEnabled  = nullptr;
  QLineEdit *m_ClsUsername = nullptr;
  QLineEdit *m_ClsPassword = nullptr;
};

#endif
