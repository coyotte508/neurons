#include <map>
#include <iostream>
#include <parallel/numeric>

#include "utils.h"
#include "macrocluster.h"

using namespace std;

void MacroCluster::clear()
{
    thinConnections(0);
}

bool MacroCluster::iterate(std::unordered_set<Fanal*> *ret)
{
    bool retVal = false;

    std::unordered_set<Fanal*> flashingFanalsBefore;
    //Store flashing fanals
    for (Cluster *c : clusters) {
        flashingFanalsBefore.insert(c->flashingFanal());
    }
    flashingFanalsBefore.erase(nullptr);

    //Second: propagate flashing
    for (Cluster *c : clusters) {
        c->propagateFlashing(nbSynapses, transmissionProbability);
    }

    //Constant input
    for (Fanal *f : inputs) {
        f->flash(Fanal::defaultFlashStrength, Fanal::defaultConnectionStrength, inputs.size());
    }

    //Third: Reset potential of neurons not flashing hard enough
    std::set<Cluster*, Cluster::hasLessStrength> byStrength;
    for (Cluster *c : clusters) {
        if (c->winnerTakeAll(minStrength)) {
            byStrength.insert(c);
        }
    }

    //Global winners take all
    globalWinnersTakeAll(byStrength);

    std::unordered_set<Fanal*> flashingResult;
    //Store flashing fanals
    for (Cluster *c : clusters) {
        flashingResult.insert(c->flashingFanal());
    }
    flashingResult.erase(nullptr);
    retVal = (flashingResult == lastFlashing);
    lastFlashing.swap(flashingResult);

    //Also flash random neurons
    if (spontaneousRelease) {
        std::uniform_real_distribution<> dist(0, 1.f);

        noise.clear();
        for (Cluster *c : clusters) {
            if (!c->flashingFanal() && dist(randg()) < spontaneousRelease) {
                noise.insert(c->randomFlash());
            }
        }
    }

    std::unordered_set<Fanal*> flashingFanalsAfter;
    //Store flashing fanals
    for (Cluster *c : clusters) {
        flashingFanalsAfter.insert(c->flashingFanal());
    }
    flashingFanalsAfter.erase(nullptr);

    //Now update connections between fanals.
    if (transmissionProbability < 0) {
        for (Fanal *f : flashingFanalsAfter) {
            for (Fanal *f2 : flashingFanalsAfter) {
                if (f == f2) {
                    continue;
                }
                f->strengthenLink(f2);
            }
            f->weakenLinks();
        }
//        for (Cluster *c :clusters) {
//            c->weakenLinks();
//        }
    }

    if (ret) {
        *ret = std::move(flashingFanalsAfter);
    }

    return retVal;
}

void MacroCluster::globalWinnersTakeAll(std::set<Cluster *, Cluster::hasLessStrength> &byStrength)
{
    if (cliqueSize == 0) {
        return;
    }

    Fanal::flash_strength minStrength = 0;
    int count = 0;
    auto it = byStrength.begin();

    if (it == byStrength.end()) {
        return;
    }

    if (nbSynapses == 1) {
        /* Deterministic approach - if some have the same score, keep them all */
        while (byStrength.size() - count > unsigned(cliqueSize)) {
            ++it;
            count++;
        }

        minStrength = (*it)->flashingFanal()->lastFlashStrength();

        while ((*byStrength.begin())->flashingFanal()->lastFlashStrength() < minStrength) {
            (*byStrength.begin())->lightDown();
            byStrength.erase(byStrength.begin());
        }
    } else {
        /* Probabilistic approach - if some have the same score, keep only what needed */
        while (byStrength.size() > unsigned(cliqueSize)) {
            (*byStrength.begin())->lightDown();
            byStrength.erase(byStrength.begin());
        }
    }
}

std::unordered_set<Fanal*> MacroCluster::getFlashingNeurons() const
{
    if (cliqueSize <= 0 || signed(lastFlashing.size()) <= cliqueSize) {
        return lastFlashing;
    }

    std::set<Fanal*, Fanal::hasLessStrength> byStrength;
    for (Fanal *f : lastFlashing) {
        byStrength.insert(f);
    }

    while (signed(byStrength.size()) > cliqueSize) {
        byStrength.erase(byStrength.begin());
    }

    return Clique(byStrength.begin(), byStrength.end());
}

std::unordered_set<Fanal*> MacroCluster::getRandomClique(int size) const
{
    if (size < 0) {
        size = cliqueSize;
    }

    return (*clusters.begin())->getRandomClique(size);
}

void MacroCluster::lightDown()
{
    for (Cluster *c : clusters) {
        c->lightDown();
    }
}

void MacroCluster::setSynapses(int nbSynapses, double transmissionProbability)
{
    this->nbSynapses = nbSynapses;
    this->transmissionProbability = transmissionProbability;

    if (nbSynapses > 1 && transmissionProbability > 0) {
        double proba = transmissionProbability;
        std::function<int(int,int)> choose;
        choose = [&choose](int k, int n) -> int {
            if (k == 0) return 1;
            return (n * choose(k - 1, n - 1)) / k;
        };
        cumulBinomial.resize(nbSynapses+1, 0);

        for (int i = 0; i <= nbSynapses; i++) {
            double p = choose(i, nbSynapses) * pow(proba, i) * pow(1-proba, nbSynapses-i);
            cumulBinomial[i] += p;

            if (i > 0) {
                cumulBinomial[i] += cumulBinomial[i-1];
            }
        }
    }
}

void MacroCluster::setCliqueSize(int size)
{
    cliqueSize = size;
}

int MacroCluster::getCliqueSize() const
{
    return cliqueSize;
}

void MacroCluster::setSpontaneousRelease(double releaseProbability)
{
    spontaneousRelease = releaseProbability;
}

void MacroCluster::setMinimumExcitation(Fanal::flash_strength connStrength)
{
    minStrength = connStrength;
}

void MacroCluster::setConstantInput(bool c)
{
    constantInput = c;
}

void MacroCluster::setMemoryEffect(bool effect)
{
    memoryEffect = effect;
}

void MacroCluster::interlink(double density)
{
    std::uniform_real_distribution<> dist(0, 1.f);

    for (Cluster *c1 : clusters) {
        for (Cluster *c2 : clusters) {
            if (c1 == c2) {
                continue;
            }

            for (int i = 0; i < c1->size(); i++) {
                for (int j = 0; j < c2->size(); j++) {
                    if (dist(randg()) < density) {
                        c1->fanal(i)->link(c2->fanal(j));
                    }
                }
            }
        }
    }
}

void MacroCluster::thinConnections(double factor)
{
    for(Cluster *c : clusters) {
        c->thinConnections(factor);
    }
}

double MacroCluster::density() const
{
    /* Incorrect algorithm in case all clusters don't have the same number of fanals, as then a weight
        for each density would be needed */
    double d = 0;

    d = __gnu_parallel::accumulate(clusters.begin(), clusters.end(), d, [](double val, Cluster *c) {
        return c->density() + val;
    });

    return d/clusters.size();
}
