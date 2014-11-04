#include <iostream>
#include <unordered_set>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "neuronsgrid.h"

typedef MacroCluster::Layer Layer;

extern int _argc;
extern char **_argv;

NeuronsGrid::NeuronsGrid() : app(_argc, _argv)
{
}

void NeuronsGrid::setMacroCluster(MacroCluster *mc)
{
    indexes.clear();
    this->mc = mc;

    auto clusters = mc->bottomLevel();

    int counter = 0;
    for (Cluster *c : clusters) {
        for (int i = 0; i < c->size(); i++) {
            indexes[c->fanal(i)] = counter++;
        }
    }

    m_clusters = mc->bottomLevel().size();
    m_fanals = (*mc->bottomLevel().begin())->size();

    emit networkSet(m_clusters, m_fanals);
}

void NeuronsGrid::setExpected(const std::unordered_set<Fanal *> &expectedFanals)
{
    this->expectedFanals = expectedFanals;
}

void NeuronsGrid::run()
{
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cpp", this);
    engine.load(QUrl(QStringLiteral("qml/neurongrid.qml")));

    app.exec();
}

void NeuronsGrid::iterate(int n)
{
    for (int i = 0; i < n-1; i++) {
        if (stack.size() > 0) {
            stack.erase(stack.begin());
        } else {
            mc->iterate();
        }
    }

    std::unordered_set<Fanal*> fanals;
    if (stack.size() > 0) {
        fanals = std::move(stack[0]);
        stack.erase(stack.begin());
    } else {
        mc->iterate(&fanals);
    }

    //update display
    QList<int> neurons;
    for (Fanal *f: fanals) {
        neurons.push_back(indexes[f]);
    }

    emit neuronsLit(neurons);
}

QList<int> NeuronsGrid::inputs() const
{
    QList<int> neurons;
    for (Fanal *f: mc->getInputs()) {
        neurons.push_back(indexes[f]);
    }

    return neurons;
}

QList<int> NeuronsGrid::expected() const
{
    QList<int> neurons;
    for (Fanal *f: expectedFanals) {
        neurons.push_back(indexes[f]);
    }

    return neurons;
}

QList<int> NeuronsGrid::noise() const
{
    QList<int> neurons;
    for (Fanal *f: mc->getNoise()) {
        neurons.push_back(indexes[f]);
    }

    return neurons;
}
