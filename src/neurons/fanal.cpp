#include <iostream>

#include "macros.h"
#include "cluster.h"
#include "fanal.h"

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

void Fanal::propragateFlash(int reduce)
{
    __gnu_parallel::for_each (links.begin(), links.end(), [this, reduce](decltype(*links.begin()) &p) {
        debug(cout << "Fanal " << this << " propagating flashing" << endl);
        p.first->flash(m_lastFlashStrength/reduce, p.second);
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
