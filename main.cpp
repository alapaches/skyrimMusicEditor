#include "mainwindow.h"
#include "changelogdialog.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/styles/skyrim_theme.qss");
    QSettings settings("SkyrimMusicEditor", "SkyrimMusicEditor");
    QString lang = settings.value("app/language", "en").toString();
    QString appVersion = "3.0";

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(styleFile.readAll());
        a.setStyleSheet(style);
        styleFile.close();
    }

    QTranslator appTranslator;
    if (lang != "en") {
        if (appTranslator.load(":/translation_" + lang + ".qm")) {
            a.installTranslator(&appTranslator);
        }
    }

    if (ChangelogDialog::shouldShow(appVersion)) {
        ChangelogDialog changelog(appVersion, lang);
        changelog.exec();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
