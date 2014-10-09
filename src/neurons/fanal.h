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
    Cluster *master() const;

    //Flash
    void flash(flash_strength str, connection_strength strength = defaultConnectionStrength, int times=1);
    void propragateFlash();
    void updateFlash();
    void removeFlash();

    flash_strength flashStrength() const;
    flash_strength lastFlashStrength() const;

    /* Links multiple fanals between each other */
    template <typename T>
    static void interlink(T list) {
        __gnu_parallel::for_each(list.begin(), list.end(), [&list](Fanal *f){
            for(auto *other : list) {
                if (f != other) {
                    f->link(other);
                }
            }
        });
    }
private:
    Cluster * owner;

    /* Value is something like intensity of connection */
    std::unordered_map<Fanal*, uint64_t> links;
    std::atomic<flash_strength> m_flashStrength, m_lastFlashStrength;
};

#endif // FANAL_H
