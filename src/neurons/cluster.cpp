#include <algorithm>
#include <parallel/algorithm>
#include <iostream>

#include "macros.h"
#include "fanal.h"
#include "cluster.h"

using namespace std;

Cluster::Cluster(int nbfanals) : flashingfanal(nullptr)
{
    fanals.resize(nbfanals);

    for (auto it = fanals.begin(); it != fanals.end(); ++it) {
        *it = new Fanal(this);
    }
}

Cluster::~Cluster()
{
    __gnu_parallel::for_each(links.begin(), links.end(), [this] (Cluster *c){
        c->removeLinks(this);
    });

    __gnu_parallel::for_each(fanals.begin(), fanals.end(), [this] (Fanal *f){
        delete f;
    });
}

void Cluster::link(Cluster *other)
{
    links.insert(other);
    other->links.insert(this);
}

void Cluster::uplink(Cluster *upper)
{
    links.insert(upper);
    upper->links.insert(this);

    uplinks.insert(upper);
    upper->downlinks.insert(this);
}

Fanal* Cluster::fanal(int index) const
{
    return fanals[index];
}

Fanal* Cluster::flashingFanal() const
{
    return flashingfanal;
}

void Cluster::propagateFlashing()
{
    if (flashingfanal) {
        debug(cout << "Cluster " << this << " propagating flashing" << endl;)
        flashingfanal->propragateFlash();
    }
}

void Cluster::winnerTakeAll()
{
    flashingfanal = nullptr;

    if (flashingfanals.empty()) {
        return;
    }

    for(Fanal *flashing : flashingfanals) {
        if (!flashingfanal) {
            flashingfanal = flashing;
            continue;
        }

        if (flashing->flashStrength() > flashingfanal->flashStrength()) {
            flashingfanal->removeFlash();
            flashingfanal = flashing;
        } else /* if (flashing != flashingfanal) */ {
            flashing->removeFlash();
        }
    };

    flashingfanals.clear();

    /* The fanal is not strong enough! */
    if (flashingfanal->flashStrength() <= Fanal::defaultFlashStrength) {
        debug(cout << "Fanal's " << flashingfanal << " strength: " << flashingfanal->flashStrength() << " is too weak (min " << Fanal::defaultFlashStrength << ")" << endl;)
        flashingfanal->removeFlash();
        flashingfanal = nullptr;
    } else {
        //Finally store the flash to propagate next time
        debug(cout << "Winner of Cluster " << this << " is fanal " << flashingfanal << " with strength " << flashingfanal->flashStrength() << endl;)
        flashingfanal->updateFlash();
    }
}

void Cluster::notifyFlashing(Fanal *f)
{
    flashingfanals.insert(f);
}

void Cluster::removeLinks(Cluster *other)
{
    /* Costly loop! */
    __gnu_parallel::for_each(fanals.begin(), fanals.end(), [this, other] (Fanal *f){
        std::for_each(other->fanals.begin(), other->fanals.end(), [f](Fanal *other){
            f->removeLink(other);
        });
    });

    links.erase(other);
    uplinks.erase(other);
    downlinks.erase(other);
}
