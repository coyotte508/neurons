#include <map>
#include "macrocluster.h"

std::unordered_set<Fanal*> MacroCluster::getFlashingNeurons() const
{
    /* Maybe the following step - to weed out weak neurons - should be done every iteration instead
     * of at the end */
    std::multimap<Fanal::flash_strength, Fanal*> flashingNeuronsByStrength;

    for (Cluster *c : clusters) {
        Fanal *f = c->flashingFanal();

        if (f) {
            flashingNeuronsByStrength.insert(std::pair<Fanal::flash_strength, Fanal*> (f->flashStrength(), f));
        }
    }

    // We remove neurons that have connections to less than one third of the rest of the network
    for (auto it = flashingNeuronsByStrength.begin(); it != flashingNeuronsByStrength.end(); ) {
        if (it->first < flashingNeuronsByStrength.size() * Fanal::defaultFlashStrength / 3) {
            it = flashingNeuronsByStrength.erase(it);
        } else {
            break;
        }
    }

    std::unordered_set<Fanal*> flashingNeurons;

    /* Convert map to set */
    auto getValue = [](decltype((*flashingNeuronsByStrength.begin())) p){
        return p.second;
    };
    std::transform(flashingNeuronsByStrength.begin(), flashingNeuronsByStrength.end(),
                    std::inserter(flashingNeurons, flashingNeurons.begin()), getValue);

    return flashingNeurons;
}
