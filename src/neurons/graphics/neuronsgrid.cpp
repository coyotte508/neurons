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

void NeuronsGrid::setCliques(const std::vector<std::unordered_set<Fanal *> > &cliques)
{
    this->cliques = cliques;
}

void NeuronsGrid::run()
{
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cpp", this);
    engine.load(QUrl(QStringLiteral("qml/neurongrid.qml")));

    app.exec();
}

void NeuronsGrid::setClique(int clique)
{
    mc->lightDown();
    mc->setInputs(cliques[clique]);
    eraseNext = true;
}

void NeuronsGrid::iterate(int n)
{
    for (int i = 0; i < n-1; i++) {
        if (stack.size() > 0) {
            stack.erase(stack.begin());
        } else {
            mc->iterate();
            lastNoise = std::move(nextNoise);
            nextNoise = mc->getNoise();
        }
    }

    std::unordered_set<Fanal*> fanals;
    if (stack.size() > 0) {
        fanals = std::move(stack[0]);
        stack.erase(stack.begin());
    } else {
        mc->iterate();
        lastNoise = std::move(nextNoise);
        nextNoise = mc->getNoise();
        fanals = mc->getFlashingNeurons();
    }

    if (eraseNext) {
        mc->setInputs(Clique());
        eraseNext = false;
    }

    //update display
    QList<int> neurons;
    for (Fanal *f: fanals) {
        neurons.push_back(indexes[f]);
    }

    lit = std::move(fanals);

    emit neuronsLit(neurons);
}

void NeuronsGrid::clear()
{
    mc->lightDown();
    mc->setInputs(Clique());
}

void NeuronsGrid::clearInputs()
{
    mc->setInputs(Clique());
}

void NeuronsGrid::addInput(int input)
{
    Clique c = mc->getInputs();
    c.insert(getFanal(input));
    mc->setInputs(c);
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
    for (Fanal *f: lastNoise) {
        neurons.push_back(indexes[f]);
    }

    return neurons;
}

int NeuronsGrid::cliqueCount() const
{
    return cliques.size();
}

QList<int> NeuronsGrid::clique(int i) const
{
    const Clique clique = cliques[i];
    QList<int> neurons;
    for (Fanal * f: clique) {
        neurons.push_back(indexes[f]);
    }

    return neurons;
}

QVariantMap NeuronsGrid::connections() const
{
    std::unordered_set<Fanal*> all;

    all.insert(lastNoise.begin(), lastNoise.end());
    all.insert(expectedFanals.begin(), expectedFanals.end());
    all.insert(mc->getInputs().begin(), mc->getInputs().end());
    all.insert(lit.begin(), lit.end());

    QVariantMap conns;

    for (Fanal *f1: all) {
        QVariantList vals;
        for (Fanal *f2 : all) {
//            if (indexes[f2] <= indexes[f1]) {
//                continue;
//            }
            if (f1->linked(f2)) {
                vals.push_back(QString::number(indexes[f2]));
            }
        }

        conns.insert(QString::number(indexes[f1]), vals);
    }

    return conns;
}

QVariantMap NeuronsGrid::allConnections() const
{
    std::unordered_set<Fanal*> all;

    auto keys = indexes.keys();
    all.insert(keys.begin(), keys.end());

    QVariantMap conns;

    for (Fanal *f1: all) {
        QVariantMap vals;
        for (Fanal *f2 : all) {
//            if (indexes[f2] <= indexes[f1]) {
//                continue;
//            }
            if (f1->linked(f2)) {
                vals.insert(QString::number(indexes[f2]), QString::number(f1->linkStrength(f2)));
            }
        }

        conns.insert(QString::number(indexes[f1]), vals);
    }

    return conns;
}

QVariantMap NeuronsGrid::connections(int neuron) const
{
    Fanal *f = getFanal(neuron);

    const auto &links = f->getLinks();

    QVariantMap ret;

    for (auto it = links.begin(); it != links.end(); ++it) {
        ret.insert(QString::number(indexes[it->first]), it->second);
    }

    return ret;
}

Fanal * NeuronsGrid::getFanal(int index) const
{
    return indexes.key(index);
}
