#include <algorithm>
#include <parallel/algorithm>
#include <iostream>
#include <random>

#include "utils.h"
#include "macros.h"
#include "fanal.h"
#include "cluster.h"

using namespace std;

Cluster::Cluster(int nbfanals)
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

bool Cluster::learnRandomClique()
{
    auto clique = getRandomClique();

    if (Fanal::interlinked(clique)) {
        return false;
    }

    Fanal::interlink(clique);
    return true;
}

bool Cluster::testRandomClique()
{
    return Fanal::interlinked(getRandomClique());
}

std::unordered_set<Fanal*> Cluster::getRandomClique() const
{
    std::unordered_set<Fanal*> clique;

    for (Cluster *c: links) {
        if (downlinks.count(c) == 0 && uplinks.count(c) == 0) {
            std::uniform_int_distribution<> dist(0, c->fanals.size() - 1);
            clique.insert(c->fanal(dist(randg())));
        }
    }
    /* Also one fanal for us */
    std::uniform_int_distribution<> dist(0, fanals.size() - 1);
    clique.insert(fanal(dist(randg())));

    return clique;
}

double Cluster::density() const
{
    int numberOfConnections (0);

    for (Fanal *f: fanals) {
        numberOfConnections += f->nbLinks();
    }

    double d = double(numberOfConnections)/(links.size()* fanals.size() * fanals.size());

    return d;
}

Fanal* Cluster::fanal(int index) const
{
    return fanals[index];
}

Fanal* Cluster::flashingFanal() const
{
    return flashingfanals.size() > 0 ? *flashingfanals.begin() : nullptr;
}

void Cluster::propagateFlashing(int nbSynapses, double transmissionProba)
{
    for(Fanal *flashing : flashingfanals) {
        debug(cout << "Cluster " << this << " propagating flashing" << endl);
        flashing->propragateFlash(nbSynapses, transmissionProba);
    }
}

void Cluster::winnerTakeAll(int minStrength)
{
    flashingfanals.clear();
    if (tempflashingfanals.empty()) {
        return;
    }

    constexpr Fanal::flash_strength variation = Fanal::defaultFlashStrength/4;

    Fanal::flash_strength maxstr = minStrength + variation;

    for(Fanal *flashing : tempflashingfanals) {
        if (flashing->flashStrength() > maxstr) {
            maxstr = flashing->flashStrength();
        }
    }

    for(Fanal *flashing : tempflashingfanals) {
        if (flashing->flashStrength() + variation < maxstr) {
            flashing->removeFlash();
        } else {
            //if (flashingfanal->flashStrength() <= Fanal::defaultFlashStrength) {
            flashingfanals.insert(flashing);
            flashing->updateFlash();
        }
    }
}

void Cluster::notifyFlashing(Fanal *f)
{
    tempflashingfanals.insert(f);
}

void Cluster::lightDown()
{
    for(Fanal *flashing : tempflashingfanals) {
        flashing->removeFlash();
    };

    tempflashingfanals.clear();

    for(Fanal *flashing : flashingfanals) {
        flashing->removeFlash();
    };

    flashingfanals.clear();
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
