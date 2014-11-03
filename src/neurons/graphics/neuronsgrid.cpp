#include <iostream>
#include <unordered_set>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "neuronsgrid.h"

typedef MacroCluster::Layer Layer;

extern int _argc;
extern char **_argv;

NeuronsGrid::NeuronsGrid() : app(_argc, _argv), mc({Layer(1000, 1)})
{
    mc.setSynapses(10, -1);
    mc.setSpontaneousRelease(0.01);
    mc.setMinimumExcitation(Fanal::defaultFlashStrength*11/10);

    auto clusters = mc.bottomLevel();

    int counter = 0;
    for (Cluster *c : clusters) {
        indexes[c->fanal(0)] = counter++;
    }

    //interlink sparsely
    Fanal::interlink(indexes.keys());
    mc.thinConnections(0.1);
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
        mc.iterate();
    }

    std::unordered_set<Fanal*> fanals;
    mc.iterate(&fanals);

    //update display
    QList<int> neurons;
    for (Fanal *f: fanals) {
        neurons.push_back(indexes[f]);
    }

    emit neuronsLit(neurons);
}
