#include "cluster.h"
#include "fanal.h"

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

void Fanal::flash(flash_strength str, connection_strength connStr, int times)
{
    /* Right now, linear law of flashing is applied. To have something maybe more realistic,
     * use something like 1-exp(-connStr*4/maxStr) ?
     *
     * That way all strong connections (maxStr/2 - maxStr) have roughly the same effect.
    */
    m_flashStrength += std::min(str, defaultFlashStrength) * connStr * times;

    owner->notifyFlashing(this);
}

void Fanal::propragateFlash()
{
    __gnu_parallel::for_each (links.begin(), links.end(), [this](decltype(*links.begin()) &p) {
        p.first->flash(m_lastFlashStrength, p.second);
    });
}

void Fanal::updateFlash()
{
    m_lastFlashStrength = (flash_strength) m_flashStrength;
    m_flashStrength = 0;
}

void Fanal::removeFlash()
{
    m_flashStrength = m_lastFlashStrength = 0;
}

Fanal::flash_strength Fanal::flashStrength() const
{
    return m_flashStrength;
}
