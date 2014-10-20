#include <map>
#include <iostream>
#include <parallel/numeric>

#include "macrocluster.h"

using namespace std;

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
        if (it->first < flashingNeuronsByStrength.size() * Fanal::defaultFlashStrength / 3) {
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
