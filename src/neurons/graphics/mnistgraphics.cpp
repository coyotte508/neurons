
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mnistgraphics.h"

extern int _argc;
extern char **_argv;

MnistGraphics::MnistGraphics(QObject *parent) :
    QObject(parent), app(_argc, _argv)
{
}

void MnistGraphics::run(const QList<QVector<int> > &states)
{
    this->states = states;

    stateIndex = 0;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cpp", this);
    engine.load(QUrl(QStringLiteral("qml/mnist.qml")));

    app.exec();
}

void MnistGraphics::setExpected(const QList<int> expected)
{
    this->expected = expected;
}

QList<int> MnistGraphics::getExpected()
{
    return expected;
}

QList<int> MnistGraphics::getState()
{
    return states[stateIndex].toList();
}

void MnistGraphics::advanceState()
{
    if (stateIndex + 1 < states.size()) {
        stateIndex += 1;
    }
}
