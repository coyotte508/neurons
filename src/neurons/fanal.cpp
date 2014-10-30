#include <iostream>

#include "utils.h"
#include "macros.h"
#include "cluster.h"
#include "fanal.h"
#include "macrocluster.h"

using namespace std;

const Fanal::connection_strength Fanal::defaultConnectionStrength;
const Fanal::flash_strength Fanal::defaultFlashStrength;

Fanal::Fanal(Cluster *owner)
      : owner(owner), m_flashStrength(0), m_lastFlashStrength(0)
{
}

void Fanal::link(Fanal *other, connection_strength strength)
{
    links[other] = strength;
}

void Fanal::removeLink(Fanal *other)
{
    auto it = links.find(other);

    if (it != links.end()) {
        links.erase(links.find(other));
    }
}

bool Fanal::linked(Fanal *other) const
{
    return links.find(other) != links.end();
}

void Fanal::strengthenLink(Fanal *other)
{
    if (linked(other)) {
        links[other] = std::min(links[other]+1, (long unsigned)(8000));
    }
}

void Fanal::weakenLink(Fanal *other)
{
    if (linked(other)) {
        links[other] = std::max(links[other]-1, (long unsigned)1);
    }
}

void Fanal::thinConnections(double factor)
{
    std::vector<Fanal*> fanals;

    for (auto it = links.begin(); it != links.end(); ++it) {
        fanals.push_back(it->first);
    }

    std::random_shuffle(fanals.begin(), fanals.end(), [](int i) {return std::uniform_int_distribution<>(0, i-1)(randg());});

    for (unsigned i = factor * fanals.size() + 1; i < fanals.size(); i++) {
        links.erase(fanals[i]);
    }
}

Cluster * Fanal::master() const
{
    return owner;
}

int Fanal::nbLinks() const
{
    return links.size();
}

void Fanal::flash(flash_strength str, connection_strength connStr, int times)
{
    /* Memory effect - carry on from last iteration */
    if (owner->owner->nbSynapses == 1 && !m_flashStrength && m_lastFlashStrength) {
        m_flashStrength += defaultFlashStrength;
    }
    /* Right now, linear law of flashing is applied. To have something maybe more realistic,
     * use something like 1-exp(-connStr*4/maxStr) ?
     *
     * That way all strong connections (maxStr/2 - maxStr) have roughly the same effect.
    */
    //TODO:: improve model of influence of flashing transmission
    debug(cout << "old flash strength for " << this << ": " << m_flashStrength << endl);
    m_flashStrength += (std::min(str, defaultFlashStrength) * connStr * times)/defaultFlashStrength;
    debug(cout << "new flash strength: " << m_flashStrength << endl);

    owner->notifyFlashing(this);
}

void Fanal::propragateFlash(int nbSynapses, double transmissionProba)
{
    __gnu_parallel::for_each (links.begin(), links.end(), [=](decltype(*links.begin()) &p) {
        debug(cout << "Fanal " << this << " propagating flashing" << endl);

        double proba = transmissionProba > 0 ? transmissionProba : sqrt(double(p.second)/8000);
        double mult = transmissionProba > 0 ? 1/transmissionProba : 1;
        std::binomial_distribution<int> distribution(nbSynapses, proba);

        p.first->flash((defaultFlashStrength * distribution(randg()) * mult) / nbSynapses, p.second);
    });
}

void Fanal::updateFlash()
{
    debug(cout << "updating flash for fanal " << this << endl);
    m_lastFlashStrength = (flash_strength) m_flashStrength;
    m_flashStrength = 0;
}

void Fanal::removeFlash()
{
    debug(cout << "removing flash for fanal " << this << endl);
    m_flashStrength = m_lastFlashStrength = 0;
}

Fanal::flash_strength Fanal::flashStrength() const
{
    return m_flashStrength;
}

Fanal::flash_strength Fanal::lastFlashStrength() const
{
    return m_lastFlashStrength;
}
