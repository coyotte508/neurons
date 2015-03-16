#ifndef EASYCLIQUENETWORK_H
#define EASYCLIQUENETWORK_H

#include <QSet>
#include <QVector>
#include <QList>
#include <QMap>
#include <functional>
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
    void setupCliqueErased(const T &data, int nerased) {
        setupClique(data);

        std::uniform_int_distribution<> dist(0, data.size()-1);

        int removed = 0;

        while (removed < nerased) {
            int x = dist(randg());

            if (activatedFanals[x].size() > 0) {
                activatedFanals[x].clear();
                removed++;
            }
        }
    }

    template <class T>
    void setupClique(const T &data) {
        activatedFanals.clear();

        auto toFanal = [&] (int i) {
            return i*nbfanals+data[i];
        };

        for (int i = 0; i < data.size(); i++) {
            activatedFanals.push_back(QSet<int>() << toFanal(i));
        }
    }

    void iterate(int it=8);

    template<class T>
    bool matchClique(const T&data) {
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

    template <class T>
    bool testCliqueErased(const T &data, int nerased) {
        setupCliqueErased(data, nerased);
        iterate();
        return matchClique(data);
    }

    void removeFanals(int value);
    void insertFanals(double ratio);
    void blurClique(const std::function<bool(int val1, int val2)> &f);
    void errorClique(double ratio);
    void fillRemaining(int value);
private:
    int nbclusters;
    int nbfanals;

    QList<QVector<int> > interConnections;
    QVector<QSet<int>> activatedFanals;
};

#endif // EASYCLIQUENETWORK_H
