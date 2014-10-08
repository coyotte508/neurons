#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <parallel/algorithm>
#include <tbb/concurrent_queue.h>

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

    /* Links multiple clusters between each other */
    template <typename T>
    static void interlink(T list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            auto it2 = it;
            ++it2;
            for ( ; it2 != list.end(); ++it2) {
                (*it)->link(*it2);
            }
        }
    }

    template <typename T>
    static void uplink(T lowlevel, T uplevel) {
        for (auto down: lowlevel) {
            for (auto up : uplevel) {
                down->uplink(up);
            }
        }
    }

    template <typename T>
    static void interuplink(T lowlevel, T uplevel) {
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
private:
    void removeLinks(Cluster *other);

    std::vector<Fanal*> fanals;
    std::unordered_set<Cluster*> links, uplinks, downlinks;

    Fanal *flashingfanal;
    tbb::concurrent_queue<Fanal *> flashingfanals;
};

#endif // CLUSTER_H
