#ifndef EASYCLIQUENETWORK_H
#define EASYCLIQUENETWORK_H

#include <QSet>
#include <QVector>
#include <QList>
#include <QMap>
#include "utils.h"

class EasyCliqueNetwork
{
public:
    EasyCliqueNetwork();

    void setSize(int nbClusters, int nbFanals);

    template<class T>
    void addClique(const T& data) {
        for(int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data.size(); j++) {
                if (i == j) {
                    continue;
                }

                interConnections[i*nbfanals+data[i]][j*nbfanals+data[j]] = interConnections[j*nbfanals+data[j]][i*nbfanals+data[i]] = 1;
                interConnections[i*nbfanals+data[i]][i*nbfanals+data[i]] = 1;
                interConnections[j*nbfanals+data[j]][j*nbfanals+data[j]] = 1;
            }
        }
    }

    template <class T>
    bool testCliqueErased(const T &data, int nerased) {
        std::uniform_int_distribution<> dist(0, data.size()-1);

        QVector<QSet<int>> activatedFanals;
        QMap<int, int> scores;

        auto toFanal = [&] (int i) {
            return i*nbfanals+data[i];
        };

        auto toCluster = [&] (int x) {
            return x/nbfanals;
        };

        for (int i = 0; i < data.size(); i++) {
            activatedFanals.push_back(QSet<int>() << toFanal(i));
        }

        int removed = 0;

        while (removed < nerased) {
            int x = dist(randg());

            if (activatedFanals[x].size() > 0) {
                activatedFanals[x].clear();
                removed++;
            }
        }

        for (int i = 0; i < 8; i++) {
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
            for(int i2 = 0; i2 < activatedFanals.size(); i2++) {
                const QSet<int> & set = activatedFanals[i2];

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

                activatedFanals[i2] = newSet;
            }
        }

        for (int i = 0; i < data.size(); i++) {
            if (*activatedFanals[i].begin() != toFanal(i)) {
                return false;
            }
        }

        return true;
    }

private:
    int nbclusters;
    int nbfanals;

    QList<QVector<int> > interConnections;
};

#endif // EASYCLIQUENETWORK_H
