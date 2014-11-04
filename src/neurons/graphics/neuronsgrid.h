#ifndef NEURONSGRID_H
#define NEURONSGRID_H

#include <unordered_set>
#include <unordered_map>
#include <QGuiApplication>
#include "../macrocluster.h"

class NeuronsGrid : public QObject
{
    Q_OBJECT

public:
    NeuronsGrid();

    void setMacroCluster(MacroCluster *mc);
    void setExpected(const std::unordered_set<Fanal*> &expectedFanals);
    void setRun(const std::vector<std::unordered_set<Fanal*>> run);
    void run();
public slots:
    void iterate(int val);
    int clusters () const { return m_clusters; }
    int fanals () const { return m_fanals; }

    QList<int> inputs () const;
    QList<int> expected () const;
    QList<int> noise () const;
signals:
    void networkSet(int nclusters, int nfanals);
    void neuronsLit(const QList<int> &neurons);
private:
    QGuiApplication app;

    MacroCluster * mc = nullptr;

    QMap<Fanal*, int> indexes;

    std::unordered_set<Fanal*> expectedFanals;
    std::vector<std::unordered_set<Fanal*>> stack;

    int m_clusters, m_fanals;
};

#endif // NEURONSGRID_H
