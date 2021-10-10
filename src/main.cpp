#include "mainwindow.hpp"

#include <QApplication>
#include <QPointer>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>

#ifndef APP_VER
#define APP_VER "1.0.0"
#endif

void LoadTranslatFile(const QStringList &translator_files)
{
    if (!translator_files.isEmpty()) {
        for (const QString &translator_file : qAsConst(translator_files)) {
            QPointer<QTranslator>translator = new QTranslator();
            if (translator->load(translator_file)) {
                if (!qApp->installTranslator(translator)) {
                    qCritical().noquote() << QString("Install translator %1 failed!").arg(translator_file);
                }
            //} else {
            //    qCritical().noquote() << QString("Load Translations %1 failed!").arg(translator_file);
            }
        }
    }
}

void InitTranslator(const QStringList &translator_files)
{
    QStringList qt_translator_files;
    qt_translator_files << QString("qt_%1").arg(QLocale::system().name());
    qt_translator_files << QString("qtbase_%1").arg(QLocale::system().name());
    for (const QString &translator_file : qAsConst(qt_translator_files)) {
        QPointer<QTranslator>translator = new QTranslator();
        if (translator->load(translator_file, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            if (!qApp->installTranslator(translator)) {
                qCritical().noquote() << QString("Install translator %1 failed!").arg(translator_file);
            }
        //} else {
        //    qCritical().noquote() << QString("Load Translations %1 failed!").arg(translator_file);
        }
    }

    if (!translator_files.isEmpty()) {
        for (const QString &translator_file : qAsConst(translator_files)) {
            QPointer<QTranslator>translator = new QTranslator();
            if (translator->load(translator_file)) {
                if (!qApp->installTranslator(translator)) {
                    qCritical().noquote() << QString("Install translator %1 failed!").arg(translator_file);
                }
            } else {
                qCritical().noquote() << QString("Load Translations %1 failed!").arg(translator_file);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("ScreenTestKit");
    QApplication::setApplicationVersion(APP_VER);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QStringList translation_files;
    QString qt_lib_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    translation_files << QString("%1/qt_%2").arg(qt_lib_path, QLocale::system().name());
    translation_files << QString("%1/qtbase_%2").arg(qt_lib_path, QLocale::system().name());
    translation_files << QString(":/i18n/%1").arg(QLocale::system().name());
    LoadTranslatFile(translation_files);

    MainWindow w;
    w.show();

    return a.exec();
}
