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

        for (const auto & layer : layerList) {
            std::unordered_set<Cluster*> newClusters;

            for (int i = 0; i < layer.nbclusters; i++) {
                newClusters.insert(new Cluster(this, layer.clustersize));
            }

            //don't waste RAM
            if (newClusters.size() < 1000) {
                Cluster::interlink(newClusters);
            }

            if (levels.size() > 0) {
                Cluster::uplink(levels.back(), newClusters);
            }

            clusters.insert(newClusters.begin(), newClusters.end());
            levels.push_back(std::move(newClusters));

            for (Cluster *c : levels.back()) {
                c->setLevel(&levels.back());
            }
        }
    }

    ~MacroCluster() {
        for (Cluster *c : clusters) {
            delete c;
        }
        clusters.clear();
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
    int testFlash(const T& neuronList, std::unordered_set<Fanal*> *_resultingNeurons=nullptr,
                   int nbIters = 5) {
        std::unordered_set<Fanal *> lastClique;

        int i;
        for (i = 0; i < nbIters + 1; i++) {
            debug(std::cout << "iteration " << i << std::endl);

            //Can parallelize all those loops
            for (Cluster *c: clusters) {
                c->propagateFlashing(nbSynapses, transmissionProbability);
            }

            if (i == 0 || (constantInput && i < nbIters) ) {
                for (Fanal *f : neuronList) {
                    f->flash(Fanal::defaultFlashStrength, Fanal::defaultConnectionStrength, neuronList.size());
                }
            } else if (constantInput && i == nbIters) {
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

            Fanal::flash_strength minStrength = 0;
            int count = 0;
            auto it = byStrength.begin();

            if (nbSynapses == 1) {
                /* Deterministic approach - if some have the same score, keep them all */
                while (cliqueSize > 0 && byStrength.size() - count > unsigned(cliqueSize)) {
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
                while (cliqueSize > 0 && byStrength.size() > unsigned(cliqueSize)) {
                    (*byStrength.begin())->lightDown();
                    byStrength.erase(byStrength.begin());
                }
            }

            //Stop the iterations if in a stable state
            std::unordered_set<Fanal*> currentClique;

            for (Cluster *c: byStrength) {
               currentClique.insert(c->flashingFanal());
            }

            if (constantInput) {
                if (lastClique == currentClique) {
                    break;
                }
            }

            lastClique.swap(currentClique);
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

        if (included) {
            return i;
        } else {
            return 0;
        }
    }

    //Do an iteration of the network
    void iterate();

    const std::unordered_set<Cluster*>& bottomLevel() const {return levels.front();}
    const std::unordered_set<Cluster*>& topLevel() const {return levels.back();}

    std::unordered_set<Fanal*> getFlashingNeurons() const;
    void lightDown(); //remove flashing neurons

    void setSynapses(int nbSynapses, double transmissionProbability);
    void setCliqueSize(int size);
    void setSpontaneousRelease(double releaseProbability);
    void setMinimumExcitation(Fanal::flash_strength connStrength = Fanal::defaultFlashStrength);
    void setConstantInput(bool);
    void thinConnections(double factor);

    double density() const;
private:
    std::vector<std::unordered_set<Cluster*>> levels;
    std::unordered_set<Cluster*> clusters;

    int nbSynapses = 1;
    double transmissionProbability = 1.f;
    int cliqueSize = -1;
    bool constantInput = true;
    double spontaneousRelease = 0;
    Fanal::flash_strength minStrength = Fanal::defaultFlashStrength/3;

    friend class Fanal;
};


#endif // MACROCLUSTER_H
