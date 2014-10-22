#ifndef MACROCLUSTER_H
#define MACROCLUSTER_H

#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>

#include "macros.h"
#include "cluster.h"
#include "fanal.h"

class Cluster;

/* A macro cluster, or cluster of clusters, but a bit more organized.
 *
 * There are several output/inputs and an internal mechanic. For example
 * if a macro clusters represents words, one output/input is individual letters,
 * and another one is cliques representing the concept of words (acting a fanals
 * inside the cluster that is the macrocluster).
 *
 * Intermediary levels can be a lot things, associations.
*/
class MacroCluster
{
public:
    template <class T>
    MacroCluster(std::initializer_list<T> layerList) {
        assert(layerList.size() > 0);

        std::unordered_set<Cluster*> lastClustersCreated;

        for (const auto & layer : layerList) {
            std::unordered_set<Cluster*> newClusters;

            for (int i = 0; i < layer.nbclusters; i++) {
                newClusters.insert(new Cluster(layer.clustersize));
            }

            Cluster::interlink(newClusters);

            if (lastClustersCreated.size() == 0) {
                bottomlevel = newClusters;
            } else {
                Cluster::uplink(lastClustersCreated, newClusters);
            }

            clusters.insert(newClusters.begin(), newClusters.end());
            lastClustersCreated = newClusters;
        }

        toplevel = lastClustersCreated;
    }

    struct Layer{
        int nbclusters;
        int clustersize;

        Layer(int nb, int size) : nbclusters(nb), clustersize(size) {
            assert(nbclusters > 0);
            assert(clustersize > 0);
        }
    };

    //add a new infon
    template <class T>
    void flash(const T& neuronList) {
        Fanal::interlink(neuronList);
    }

    //test if an infon is in memory, also return activated neurons if non-null ptr
    template <class T>
    bool testFlash(const T& neuronList, std::unordered_set<Fanal*> *_resultingNeurons=nullptr,
                   int nbIters = 5) {
        for (int i = 0; i < nbIters + 1; i++) {
            debug(std::cout << "iteration " << i << std::endl);

            //Can parallelize all those loops
            for (Cluster *c: clusters) {
                c->propagateFlashing(nbSynapses, transmissionProbability);
            }

            if (i < nbIters) {
                for (Fanal *f : neuronList) {
                    f->flash(Fanal::defaultFlashStrength, Fanal::defaultConnectionStrength, neuronList.size());
                }
            } else if (i == nbIters) {
                //Last try with lot less excitation (to remove unworthy inputs)
                for (Fanal *f : neuronList) {
                    f->flash(Fanal::defaultFlashStrength*4/5, Fanal::defaultConnectionStrength);
                }
            }

            std::set<Cluster*, Cluster::hasLessStrength> byStrength;

            for (Cluster *c: clusters) {
                if (c->winnerTakeAll(Fanal::defaultFlashStrength/3)) {
                    byStrength.insert(c);
                }
            }

            while (cliqueSize > 0 && byStrength.size() > cliqueSize) {
                (*byStrength.begin())->lightDown();
                byStrength.erase(byStrength.begin());
            }
        }

        auto resultingNeurons = getFlashingNeurons();

        if (_resultingNeurons) {
            //good occasion to std move
            *_resultingNeurons = resultingNeurons;
        }

        lightDown();

        bool included = true;

        for (Fanal *f : neuronList) {
            if (resultingNeurons.find(f) == resultingNeurons.end()) {
                included = false;
                break;
            }
        }

        return included;
    }

    const std::unordered_set<Cluster*>& bottomLevel() const {return bottomlevel;}
    const std::unordered_set<Cluster*>& topLevel() const {return toplevel;}

    std::unordered_set<Fanal*> getFlashingNeurons() const;
    void lightDown(); //remove flashing neurons

    void setSynapses(int nbSynapses, double transmissionProbability);
    void setCliqueSize(int size);
    void setSpontaneousRelease(double releaseProbability);

    double density() const;
private:
    std::unordered_set<Cluster*> bottomlevel;
    std::unordered_set<Cluster*> toplevel;
    std::unordered_set<Cluster*> clusters;

    int nbSynapses = 1;
    double transmissionProbability = 1.f;
    int cliqueSize = -1;
};


#endif // MACROCLUSTER_H
