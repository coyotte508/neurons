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
#include "utils.h"

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
                   int nbIters = 5, int successiveIter = 2) {
        setInputs(neuronList);
        setMinimumExcitation(Fanal::defaultFlashStrength/3);

        int i;
        int succ = 1;
        for (i = 0; i < nbIters + 1; i++) {
            debug(std::cout << "iteration " << i << std::endl);

            if (iterate()) {
                succ ++;

                if (succ >= successiveIter) {
                    break;
                }
            } else {
                succ = 1;
            }
        }

        if (_resultingNeurons) {
            //good occasion to std move
            *_resultingNeurons = getFlashingNeurons();
        }

        lightDown();

        return i;
    }

    //Remove all connections
    void clear();

    //Do an iteration of the network, returns true if flashing network is unchanged
    bool iterate(std::unordered_set<Fanal*> *flashingFanals= nullptr);
    //Takes cliqueSize clusters
    void globalWinnersTakeAll(std::set<Cluster*, Cluster::hasLessStrength> &cl);

    const std::unordered_set<Cluster*>& bottomLevel() const {return levels.front();}
    const std::unordered_set<Cluster*>& topLevel() const {return levels.back();}

    std::unordered_set<Fanal*> getFlashingNeurons() const;
    std::unordered_set<Fanal*> getRandomClique(int size=-1) const;
    void lightDown(); //remove flashing neurons

    void setSynapses(int nbSynapses, double transmissionProbability);
    void setCliqueSize(int size);
    int getCliqueSize() const;
    void setSpontaneousRelease(double releaseProbability);
    void setMinimumExcitation(Fanal::flash_strength connStrength = Fanal::defaultFlashStrength);
    void setConstantInput(bool);
    void setMemoryEffect(bool);

    void interlink(double density);
    void thinConnections(double factor);

    void setEpsilon(double epsilon) {
        this->epsilon = epsilon;
    }
    void setMu(double mu) {
        this->mu = mu;
    }

    template <class T>
    void setInputs(T&& inputs) {
        this->inputs = std::forward<T>(inputs);
    }
    const std::unordered_set<Fanal*> &getInputs() const {
        return inputs;

    }
    const std::unordered_set<Fanal*> &getNoise() const {
        return noise;
    }

    double density() const;
private:
    std::vector<std::unordered_set<Cluster*>> levels;
    std::unordered_set<Cluster*> clusters;

    std::unordered_set<Fanal*> inputs;
    std::unordered_set<Fanal*> noise;
    std::unordered_set<Fanal*> lastFlashing;

    int nbSynapses = 1;
    double transmissionProbability = 1.f;
    double epsilon = 0.141f;
    double mu = 10;
    int cliqueSize = -1;
    bool constantInput = true;
    bool memoryEffect = true;
    double spontaneousRelease = 0;
    Fanal::flash_strength minStrength = 0;
    std::vector<double> cumulBinomial;

    friend class Fanal;
};


#endif // MACROCLUSTER_H
