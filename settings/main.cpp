#include "backend.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QUrl>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    Settings settings;
    QQmlApplicationEngine engine;
    QTranslator translator;
    static_cast<void>(translator.load(":/i18n/settings_tr"));
    QString applied;
    auto apply_language = [&settings, &translator, &engine, &applied]() {
        QString lang = settings.property("language").toString();
        if (lang == applied)
            return;
        applied = lang;
        if (lang == "tr")
            QCoreApplication::installTranslator(&translator);
        else
            QCoreApplication::removeTranslator(&translator);
        engine.retranslate();
    };
    apply_language();
    QObject::connect(&settings, &Settings::changed, &app, apply_language);
    (*engine.rootContext()).setContextProperty("settings", &settings);
    engine.load(QUrl("qrc:/settings.qml"));
    return app.exec();
}
