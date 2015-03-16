#include "easycliquenetwork.h"

EasyCliqueNetwork::EasyCliqueNetwork()
{
}

void EasyCliqueNetwork::setSize(int nbClusters, int nbFanals)
{
    nbclusters = nbClusters;
    nbfanals = nbFanals;

    interConnections.clear();

    for(int i = 0; i < nbClusters*nbFanals; i++) {
        interConnections.push_back(QVector<int>(nbFanals*nbClusters, 0));
    }
}

void EasyCliqueNetwork::iterate(int nbIt)
{
    QMap<int, int> scores;

    auto toCluster = [&] (int x) {
        return x/nbfanals;
    };

    for (int it = 0; it < nbIt; it++) {
        scores.clear();

        //add scores each iteration
        foreach (const QSet<int>& set, activatedFanals) {
            foreach(int x, set) {
                for (int j = 0; j < interConnections[x].size(); j++) {
                    if (interConnections[x][j]) {
                        scores[j] += 1;
                        activatedFanals[toCluster(j)].insert(j);
                    }
                }
            }
        }

        //pick best scores amongs each set
        for(int i = 0; i < activatedFanals.size(); i++) {
            const QSet<int> & set = activatedFanals[i];

            int maxScore = 0;

            QSet<int> newSet;

            foreach(int j, set) {
                if (scores[j] > maxScore) {
                    maxScore = scores[j];
                    newSet.clear();
                    newSet.insert(j);
                } else if (scores[j] == maxScore) {
                    newSet.insert(j);
                }
            }

            activatedFanals[i] = newSet;
        }
    }
}

void EasyCliqueNetwork::removeFanals(int value)
{
    for(int i = 0; i < activatedFanals.size(); i++) {
        activatedFanals[i].remove(i*nbfanals + value);
    }
}

void EasyCliqueNetwork::insertFanals(double ratio)
{
    std::uniform_int_distribution<> fanalDist(0, nbfanals-1);
    QList<QSet<int>*> empty;

    for (int i = 0; i < activatedFanals.size(); i++) {
        if (activatedFanals[i].isEmpty()) {
            empty.push_back(&activatedFanals[i]);
        }
    }

    std::random_shuffle(empty.begin(), empty.end());

    for (int i = 0; i < empty.size() * ratio; i++) {
        empty[i]->insert(fanalDist(randg()));
    }
}

void EasyCliqueNetwork::blurClique(const std::function<bool(int val1, int val2)> &isBlurred)
{
    for (int i = 0; i < activatedFanals.size(); i++) {
        if (!activatedFanals[i].isEmpty()) {
            for (int j = 0; j < nbfanals; j++) {
                if (isBlurred(j, (*activatedFanals[i].begin())%nbfanals)) {
                    activatedFanals[i].insert(j+i*nbfanals);
                }
            }
        }
    }
}

void EasyCliqueNetwork::errorClique(double ratio)
{
    std::uniform_int_distribution<> fanalDist(0, nbfanals-1);
    QList<QSet<int>*> full;

    for (int i = 0; i < activatedFanals.size(); i++) {
        if (!activatedFanals[i].isEmpty()) {
            full.push_back(&activatedFanals[i]);
        }
    }

    std::random_shuffle(full.begin(), full.end());

    for (int i = 0; i < full.size() * ratio; i++) {
        full[i]->clear();
        full[i]->insert(fanalDist(randg()));
    }
}

void EasyCliqueNetwork::fillRemaining(int value)
{
    for (int i = 0; i < activatedFanals.size(); i++) {
        if (activatedFanals[i].isEmpty()) {
            activatedFanals[i].insert(i*nbfanals+value);
        }
    }
}
