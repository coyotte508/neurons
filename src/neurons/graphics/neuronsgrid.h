#ifndef NEURONSGRID_H
#define NEURONSGRID_H

#include <unordered_map>
#include <QGuiApplication>
#include "../macrocluster.h"

class NeuronsGrid : public QObject
{
    Q_OBJECT
public:
    NeuronsGrid();

    void run();
public slots:
    void iterate(int val);
signals:
    void neuronsLit(const QList<int> &neurons);
private:
    QGuiApplication app;

    MacroCluster mc;

    QMap<Fanal*, int> indexes;
};

#endif // NEURONSGRID_H
