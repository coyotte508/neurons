#include <map>
#include <iostream>
#include <parallel/numeric>

#include "utils.h"
#include "macrocluster.h"

using namespace std;

void MacroCluster::iterate(std::unordered_set<Fanal*> *ret)
{
    std::unordered_set<Fanal*> flashingFanals;
    //Store flashing fanals
    for (Cluster *c : clusters) {
        flashingFanals.insert(c->flashingFanal());
    }
    flashingFanals.erase(nullptr);

    //Second: propagate flashing
    for (Cluster *c : clusters) {
        c->propagateFlashing(nbSynapses, transmissionProbability);
    }

    //Third: Reset potential of neurons not flashing hard enough
    for (Cluster *c : clusters) {
        c->winnerTakeAll(minStrength);
    }

    //Also flash random neurons
    if (spontaneousRelease) {
        std::uniform_real_distribution<> dist(0, 1.f);

        for (Cluster *c : clusters) {
            if (dist(randg()) < spontaneousRelease) {
                c->randomFlash();
            }
        }
    }

    std::unordered_set<Fanal*> flashingFanals2;
    //Store flashing fanals
    for (Cluster *c : clusters) {
        flashingFanals2.insert(c->flashingFanal());
    }
    flashingFanals2.erase(nullptr);

    //Now update connections between fanals.
    for (Fanal *f : flashingFanals) {
        for (Fanal *f2 : flashingFanals2) {
            if (flashingFanals.find(f2) == flashingFanals.end()) {
                f->strengthenLink(f2);
            }

            if (flashingFanals2.find(f) == flashingFanals2.end()) {
                f2->weakenLink(f);
            }
        }
    }

    if (ret) {
        *ret = std::move(flashingFanals2);
    }
}

std::unordered_set<Fanal*> MacroCluster::getFlashingNeurons() const
{
    /* Maybe the following step - to weed out weak neurons - should be done every iteration instead
     * of at the end */
    std::multimap<Fanal::flash_strength, Fanal*> flashingNeuronsByStrength;

    for (Cluster *c : clusters) {
        Fanal *f = c->flashingFanal();

        if (f) {
            flashingNeuronsByStrength.insert(std::pair<Fanal::flash_strength, Fanal*> (f->lastFlashStrength(), f));
        }
    }


    debug(cout << "Found " << flashingNeuronsByStrength.size() << " neurons somewhat flashing " << endl);

    // We remove neurons that have connections to less than one third of the rest of the network
    for (auto it = flashingNeuronsByStrength.begin(); it != flashingNeuronsByStrength.end(); ) {
        bool tooMany = flashingNeuronsByStrength.size() > unsigned(cliqueSize); // if -1, always false
        if (tooMany || it->first < flashingNeuronsByStrength.size() * Fanal::defaultFlashStrength / 3) {
            it = flashingNeuronsByStrength.erase(it);
        } else {
            break;
        }
    }

    debug(cout << "Kept " << flashingNeuronsByStrength.size() << " neurons flashing " << endl);

    std::unordered_set<Fanal*> flashingNeurons;

    /* Convert map to set */
    auto getValue = [](decltype((*flashingNeuronsByStrength.begin())) p){
        return p.second;
    };
    std::transform(flashingNeuronsByStrength.begin(), flashingNeuronsByStrength.end(),
                    std::inserter(flashingNeurons, flashingNeurons.begin()), getValue);

    return flashingNeurons;
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
}

void MacroCluster::setCliqueSize(int size)
{
    cliqueSize = size;
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
