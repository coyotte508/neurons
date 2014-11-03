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

    emit networkSet(mc->bottomLevel().size(), (*mc->bottomLevel().begin())->size());
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
        mc->iterate();
    }

    std::unordered_set<Fanal*> fanals;
    mc->iterate(&fanals);

    //update display
    QList<int> neurons;
    for (Fanal *f: fanals) {
        neurons.push_back(indexes[f]);
    }

    emit neuronsLit(neurons);
}
