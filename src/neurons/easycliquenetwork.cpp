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
