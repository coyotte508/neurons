#ifndef MACROCLUSTER_H
#define MACROCLUSTER_H

#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <iostream>

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
    void flash(T neuronList) {
        Fanal::interlink(neuronList);
    }

    //test if an infon is in memory, also return activated neurons if non-null ptr
    template <class T>
    bool testFlash(T neuronList, std::unordered_set<Fanal*> *_resultingNeurons=nullptr) {
        for (int i = 0; i < 5; i++) {
            debug(std::cout << "iteration " << i << std::endl;)

            //Can parallelize all those loops
            for (Cluster *c: clusters) {
                c->propagateFlashing();
            }

            for (Fanal *f : neuronList) {
                f->flash(Fanal::defaultFlashStrength, Fanal::defaultConnectionStrength, neuronList.size());
            }

            for (Cluster *c: clusters) {
                c->winnerTakeAll();
            }
        }

        {
            debug(std::cout << "final iteration"<< std::endl;)
            for (Cluster *c: clusters) {
                c->propagateFlashing();
            }

            //Last try with lot less excitation (to remove unworthy inputs)
            for (Fanal *f : neuronList) {
                f->flash(Fanal::defaultFlashStrength*4/5, Fanal::defaultConnectionStrength);
            }

            for (Cluster *c: clusters) {
                c->winnerTakeAll();
            }
        }

        auto resultingNeurons = getFlashingNeurons();

        if (_resultingNeurons) {
            //good occasion to std move
            *_resultingNeurons = resultingNeurons;
        }

        return std::includes(resultingNeurons.begin(), resultingNeurons.end(),
                             neuronList.begin(), neuronList.end());
    }

    const std::unordered_set<Cluster*>& bottomLevel() const {return bottomlevel;}
    const std::unordered_set<Cluster*>& topLevel() const {return toplevel;}

    std::unordered_set<Fanal*> getFlashingNeurons() const;
private:
    std::unordered_set<Cluster*> bottomlevel;
    std::unordered_set<Cluster*> toplevel;
    std::unordered_set<Cluster*> clusters;
};


#endif // MACROCLUSTER_H
