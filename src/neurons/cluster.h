#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <parallel/algorithm>
#include <tbb/concurrent_unordered_set.h>

class Fanal;

/* Cluster of fanals */
class Cluster
{
public:
    Cluster(int nbfanals=0);
    ~Cluster();

    /* Links two clusters on the same level */
    void link(Cluster *other);
    /* Links this cluster (lower level) to the given cluster (upper level) */
    void uplink(Cluster *upper);

    /* Learn a random clique (only in this level, not other levels for now) */
    bool learnRandomClique();
    /* Test a random clique (only in this level, not other levels for now) */
    bool testRandomClique();
    /* Get a random clique (only in this level, not other levels for now) */
    std::unordered_set<Fanal*> getRandomClique() const;

    /* Density of the cluster (number of outgoing connections of each fanal / number of linked clusters * numbers of fanals^2)
     * Incorrect when clusters of different number of fanals are present */
    double density() const;

    /* Links multiple clusters between each other */
    template <typename T>
    static void interlink(const T& list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            auto it2 = it;
            ++it2;
            for ( ; it2 != list.end(); ++it2) {
                (*it)->link(*it2);
            }
        }
    }

    template <typename T>
    static void uplink(const T& lowlevel, const T& uplevel) {
        for (auto down: lowlevel) {
            for (auto up : uplevel) {
                down->uplink(up);
            }
        }
    }

    template <typename T>
    static void interuplink(const T& lowlevel, const T& uplevel) {
        interlink(lowlevel);
        interlink(uplevel);
        uplink(lowlevel, uplevel);
    }

    Fanal* fanal(int index) const;
    Fanal* flashingFanal() const;

    void propagateFlashing();
    void winnerTakeAll();

    //Called by a fanal when it flashes
    void notifyFlashing(Fanal * f);
    //remove flashing
    void lightDown();
private:
    void removeLinks(Cluster *other);

    std::vector<Fanal*> fanals;
    std::unordered_set<Cluster*> links, uplinks, downlinks;

    std::unordered_set<Fanal *> flashingfanals;
    tbb::concurrent_unordered_set<Fanal *> tempflashingfanals;
};

#endif // CLUSTER_H
