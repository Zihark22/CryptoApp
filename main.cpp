#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "qmltranslator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlTranslator qmlTranslator;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("qmlTranslator", &qmlTranslator);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
