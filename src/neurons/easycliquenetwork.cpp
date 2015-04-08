#include <QDebug>
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

void EasyCliqueNetwork::iterate(int nbIt, int minScore, bool guided)
{
    QMap<int, int> scores;

    auto toCluster = [&] (int x) {
        return x/nbfanals;
    };

    if (debug) {
        debugStates.clear();

        QVector<int> fanals;
        foreach (const QSet<int>& set, activatedFanals) {
            if (!set.isEmpty()) {
                fanals.push_back(*set.begin());
            }
        }

        debugStates.push_back(fanals);

        //qDebug() << debugStates;
    }

    for (int it = 0; it < nbIt; it++) {
        scores.clear();

        //add scores each iteration
        foreach (const QSet<int>& set, activatedFanals) {
            foreach(int x, set) {
                for (int j = 0; j < interConnections[x].size(); j++) {
                    if (interConnections[x][j]) {
                        scores[j] += 1;
                        int destCluster = toCluster(j);

                        if (!guided || guide.contains(destCluster)) {
                            activatedFanals[destCluster].insert(j);
                        }
                    }
                }
            }
        }

        //pick best scores amongs each set
        for(int i = 0; i < activatedFanals.size(); i++) {
            const QSet<int> & set = activatedFanals[i];

            int maxScore = minScore;

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

        if (debug) {
            QVector<int> fanals;
            foreach (const QSet<int>& set, activatedFanals) {
                if (!set.isEmpty()) {
                    fanals.push_back(*set.begin());
                }
            }

            debugStates.push_back(fanals);
            //qDebug() << fanals;
        }
    }
}

void EasyCliqueNetwork::losersTakeOut(int minScore)
{
    QMap<int, int> scores;
    bool cleared = false;

    do {
        scores.clear();

        //add scores
        foreach (const QSet<int>& set, activatedFanals) {
            foreach(int x, set) {
                for (int j = 0; j < interConnections[x].size(); j++) {
                    if (interConnections[x][j]) {
                        scores[j] += 1;
                    }
                }
            }
        }
        cleared = false;

        for (int i = 0; i < nbclusters; i++) {
            if (!activatedFanals[i].isEmpty() && scores[*activatedFanals[i].begin()] < minScore) {
                activatedFanals[i].clear();
                cleared = true;
            }
        }
    } while (cleared == true);
}

void EasyCliqueNetwork::fixGuide()
{
    guide.clear();
    for(int i = 0; i < activatedFanals.size(); i++) {
        if (!activatedFanals[i].isEmpty()) {
            guide.insert(i);
        }
    }
}

bool EasyCliqueNetwork::matchClique(QList<int> data)
{
    qSort(data);
    //qDebug() << "true match clique";
    //qDebug() << data;
   for (int i = 0; i < data.size(); i++) {
       if (*activatedFanals[i].begin() != data[i]) {
           return false;
       }
   }

   return true;
}

bool EasyCliqueNetwork::matchClique(const QByteArray &data)
{
    auto toFanal = [&] (int i) {
        return i*nbfanals+data[i];
    };

    for (int i = 0; i < data.size(); i++) {
        if (*activatedFanals[i].begin() != toFanal(i)) {
            return false;
        }
    }

    return true;
}

void EasyCliqueNetwork::removeFanals(int value)
{
    for(int i = 0; i < activatedFanals.size(); i++) {
        activatedFanals[i].remove(i*nbfanals + value);
    }
}

void EasyCliqueNetwork::removeConnectionsToFanals(int value)
{
    for (int i = 0; i < nbclusters * nbfanals; i++) {
        for (int j = 0; j < nbclusters; j++) {
            interConnections[i][nbfanals*j+value] = 0;
        }
    }
}

void EasyCliqueNetwork::insertFanals(double ratio)
{
    std::uniform_int_distribution<> fanalDist(0, nbfanals-1);
    QList<QSet<int>*> empty;
    QMap<QSet<int>*, int> indexes;

    for (int i = 0; i < activatedFanals.size(); i++) {
        if (activatedFanals[i].isEmpty()) {
            empty.push_back(&activatedFanals[i]);
            indexes[&activatedFanals[i]] = i;
        }
    }

    std::random_shuffle(empty.begin(), empty.end());

    for (int i = 0; i < empty.size() * ratio; i++) {
        empty[i]->insert(indexes[empty[i]]*nbfanals +fanalDist(randg()));
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
    QMap<QSet<int>*, int> indexes;

    for (int i = 0; i < activatedFanals.size(); i++) {
        if (!activatedFanals[i].isEmpty()) {
            full.push_back(&activatedFanals[i]);
            indexes[&activatedFanals[i]] = i;
        }
    }

    std::random_shuffle(full.begin(), full.end());

    for (int i = 0; i < full.size() * ratio; i++) {
        full[i]->clear();
        full[i]->insert(indexes[full[i]]*nbfanals +fanalDist(randg()));
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

void EasyCliqueNetwork::setConnections(const QHash<int, QSet<int> > &connections)
{
    foreach(int key, connections.keys()) {
        interConnections[key][key] = 1;

        foreach (int v, connections[key]) {
            interConnections[key][v] = 1;
        }
    }
}
