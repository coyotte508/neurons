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

    void setMacroCluster(MacroCluster *mc);
    void run();
public slots:
    void iterate(int val);
signals:
    void networkSet(int nclusters, int nfanals);
    void neuronsLit(const QList<int> &neurons);
private:
    QGuiApplication app;

    MacroCluster * mc = nullptr;

    QMap<Fanal*, int> indexes;
};

#endif // NEURONSGRID_H
