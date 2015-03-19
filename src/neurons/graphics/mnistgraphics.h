#ifndef MNISTGRAPHICS_H
#define MNISTGRAPHICS_H

#include <QGuiApplication>
#include <QObject>
#include <QList>
#include <QVector>

class MnistGraphics : public QObject
{
    Q_OBJECT
public:
    explicit MnistGraphics(QObject *parent = 0);

    void run(const QList<QVector<int>> &states);
    void setExpected(const QList<int> expected);
signals:

public slots:
    QList<int> getExpected();
    QList<int> getState();
    void advanceState();

private:
    int stateIndex = 0;
    QList<QVector<int>> states;
    QList<int> expected;

    QGuiApplication app;
};

#endif // MNISTGRAPHICS_H
