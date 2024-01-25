#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QUrlQuery>

#include <QRetro.h>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("Pleasant");
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

  QCommandLineParser parser;
  QCommandLineOption option_uri(QStringList() << "uri", "URI to process", "uri");
  parser.addOption(option_uri);
  parser.process(app);

  QUrl url(parser.value(option_uri));
  if (url.isValid())
  {
    QString command = url.host();

    if (command == "launch")
    {
      QUrlQuery query(url);
      auto game_id = query.queryItemValue("game_id").toUInt();

      if (game_id)
      {

      }
    }
  }

  MainWindow w;
  w.show();

  return app.exec();
}
