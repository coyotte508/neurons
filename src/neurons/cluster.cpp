#include <algorithm>
#include <parallel/algorithm>
#include <iostream>
#include <random>

#include "utils.h"
#include "macros.h"
#include "fanal.h"
#include "cluster.h"

using namespace std;

Cluster::Cluster(MacroCluster *owner, int nbfanals) : owner(owner)
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

void Cluster::thinConnections(double factor)
{
    for (Fanal *f : fanals) {
        f->thinConnections(factor);
    }
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

std::unordered_set<Fanal*> Cluster::getRandomClique(int size) const
{
    std::unordered_set<Fanal*> clique;

    std::vector<const Cluster*> sameLevel;

    for (Cluster *c: *level) {
        sameLevel.push_back(c);
    }

    /* Shuffle the list randomly and take the `size` first elements */
    if (size != -1) {
        std::random_shuffle (sameLevel.begin(), sameLevel.end(), [](int i) {return std::uniform_int_distribution<>(0, i-1)(randg());});
    } else {
        //taking all elements, no need to shuffle
        size = sameLevel.size();
    }

    for (int i = 0; i < size; i++) {
        const Cluster *c = sameLevel[i];

        std::uniform_int_distribution<> dist(0, c->fanals.size() - 1);
        clique.insert(c->fanal(dist(randg())));
    }

    return clique;
}

double Cluster::density() const
{
    int numberOfConnections (0);

    for (Fanal *f: fanals) {
        numberOfConnections += f->nbLinks();
    }

    int lsize = links.size() ? links.size() : level->size();

    double d = double(numberOfConnections)/(lsize * fanals.size() * fanals.size());

    return d;
}

void Cluster::setLevel(std::unordered_set<Cluster *> *level)
{
    this->level = level;
}

int Cluster::size() const
{
    return fanals.size();
}

Fanal* Cluster::fanal(int index) const
{
    return fanals[index];
}

Fanal* Cluster::flashingFanal() const
{
    if (flashingfanals.size() > 0) {
        Fanal *ret = nullptr;

        for (Fanal *f : flashingfanals) {
            if (!ret || f->lastFlashStrength() > ret->lastFlashStrength()) {
                ret = f;
            }
        }
        return ret;
    } else {
        return nullptr;
    }
    return flashingfanals.size() > 0 ? *flashingfanals.begin() : nullptr;
}

void Cluster::propagateFlashing(int nbSynapses, double transmissionProba)
{
    for(Fanal *flashing : flashingfanals) {
        debug(cout << "Cluster " << this << " propagating flashing" << endl);
        flashing->propragateFlash(nbSynapses, transmissionProba);
    }
}

bool Cluster::winnerTakeAll(int minStrength)
{
    flashingfanals.clear();
    if (tempflashingfanals.empty()) {
        return false;
    }

    constexpr Fanal::flash_strength variation = 0;

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

    tempflashingfanals.clear();

    return flashingfanals.size() > 0;
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

Fanal * Cluster::randomFlash()
{
    auto index = std::uniform_int_distribution<>(0, fanals.size()-1)(randg());
    Fanal *f = fanals[index];

    f->flash(Fanal::defaultFlashStrength, Fanal::defaultConnectionStrength);
    f->updateFlash(true);
    flashingfanals.insert(f);

    return f;
}

void Cluster::weakenLinks()
{
    for (Fanal *f : fanals) {
        f->weakenLinks();
    }
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

bool Cluster::hasLessStrength::operator ()(const Cluster *a, const Cluster *b) {
    Fanal::flash_strength sa = a->flashingFanal()->lastFlashStrength();
    Fanal::flash_strength sb = b->flashingFanal()->lastFlashStrength();

    return sa < sb || (sa == sb && a < b);
}
