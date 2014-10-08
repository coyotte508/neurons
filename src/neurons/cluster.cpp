#include <algorithm>
#include <parallel/algorithm>

#include "fanal.h"
#include "cluster.h"

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
        flashingfanal->propragateFlash();
    }
}

void Cluster::winnerTakeAll()
{
    flashingfanal = nullptr;
    flashingfanals.try_pop(flashingfanal);

    Fanal *test = nullptr;
    while(flashingfanals.try_pop(test)) {
        if (test->flashStrength() > flashingfanal->flashStrength()) {
            flashingfanal->removeFlash();
            flashingfanal = test;
        } else {
            test->removeFlash();
        }
    }

    if (!flashingfanal) {
        return;
    }

    /* The fanal is not strong enough! */
    if (flashingfanal->flashStrength() <= Fanal::defaultFlashStrength) {
        flashingfanal->removeFlash();
        flashingfanal = nullptr;
    } else {
        //Finally store the flash to propagate next time
        flashingfanal->updateFlash();
    }
}

void Cluster::notifyFlashing(Fanal *f)
{
    flashingfanals.push(f);
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
