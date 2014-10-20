#ifndef FANAL_H
#define FANAL_H

#include <cstdint>
#include <unordered_map>
#include <parallel/algorithm>
#include <atomic>

class Cluster;

/*
 * Representation of 100-something neurons
 *
 * Fanals belong to a cluster. In a cluster, only one fanal is activated at a time. Information
 * is represented by fully interconnected fanals of different clusters.
 */
class Fanal
{
public:
    typedef u_int64_t connection_strength;
    static const connection_strength defaultConnectionStrength = 256;
    typedef u_int64_t flash_strength;
    static const flash_strength defaultFlashStrength = 256;

    Fanal(Cluster *owner=nullptr);

    void link(Fanal *other, connection_strength strength = defaultConnectionStrength);
    void removeLink(Fanal* other);
    bool linked(Fanal* other) const;
    Cluster *master() const;
    int nbLinks() const;

    //Flash
    void flash(flash_strength str, connection_strength strength = defaultConnectionStrength, int times=1);
    void propragateFlash(int nbSynapses, double transmissionProba);
    void updateFlash();
    void removeFlash();

    flash_strength flashStrength() const;
    flash_strength lastFlashStrength() const;

    /* Links multiple fanals between each other */
    template <typename T>
    static void interlink(const T& list) {
        __gnu_parallel::for_each(list.begin(), list.end(), [&list](Fanal *f){
            for(auto *other : list) {
                if (f != other) {
                    f->link(other);
                }
            }
        });
    }

    /* Test if a clique is fully interlinked */
    template <typename T>
    static bool interlinked(const T& list) {
        /* Try to find two fanals not linked together */
        auto it = __gnu_parallel::find_if(list.begin(), list.end(), [&list](Fanal *f){
            for(auto *other : list) {
                if (f != other && !f->linked(other)) {
                    return true;
                }
            }

            return false;
        });

        /* If we had found two fanals not linked together, it would be before list.end() */
        return it == list.end();
    }

private:
    Cluster * owner;

    /* Value is something like intensity of connection */
    std::unordered_map<Fanal*, uint64_t> links;
    std::atomic<flash_strength> m_flashStrength, m_lastFlashStrength;
};

#endif // FANAL_H
