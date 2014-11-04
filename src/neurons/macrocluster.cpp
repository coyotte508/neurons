#include <map>
#include <iostream>
#include <parallel/numeric>

#include "utils.h"
#include "macrocluster.h"

using namespace std;

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
        for (Fanal *f : flashingFanalsBefore) {
            for (Fanal *f2 : flashingFanalsAfter) {
                if (flashingFanalsBefore.find(f2) == flashingFanalsAfter.end()) {
                    f->strengthenLink(f2);
                }

                if (flashingFanalsBefore.find(f) == flashingFanalsAfter.end()) {
                    f2->weakenLink(f);
                }
            }
        }
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
