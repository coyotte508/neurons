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
    void setRun(const std::vector<std::unordered_set<Fanal*>> &run);
    void setCliques(const std::vector<std::unordered_set<Fanal*>> &cliques);
    void run();
public slots:
    void setClique(int clique);
    void iterate(int val);
    void clear();
    int clusters () const { return m_clusters; }
    int fanals () const { return m_fanals; }

    QList<int> inputs () const;
    QList<int> expected () const;
    QList<int> noise () const;
    QList<int> clique (int i) const;
    int cliqueCount() const;
    QVariantMap connections () const;
    QVariantMap connections (int neuron) const;
signals:
    void networkSet(int nclusters, int nfanals);
    void neuronsLit(const QList<int> &neurons);
private:
    Fanal *getFanal(int index) const;

    QGuiApplication app;

    MacroCluster * mc = nullptr;

    QMap<Fanal*, int> indexes;

    std::unordered_set<Fanal*> lit;
    std::unordered_set<Fanal*> expectedFanals;
    std::unordered_set<Fanal*> lastNoise, nextNoise;
    std::vector<std::unordered_set<Fanal*>> stack, cliques;

    int m_clusters, m_fanals;
    bool eraseNext = false;
};

#endif // NEURONSGRID_H
