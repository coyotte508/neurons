#include <QCoreApplication>

#include <QSet>
#include <QMap>
#include <random>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    srand(time(NULL));

    int nbTests(0);
    int nbErrors(0);

    for (int network = 0; network < 40; network++) {
        cout << "Network " << (network + 1) << endl;

        bool connections[2048][2048] = {0};
        bool reverseConnections[2048][2048] = {0};
        bool interConnectionsLayer2[2048][2048] = {0};

        QList<QSet<int> > cliques;
        QList<QSet<int> > cliques2;

        /* initialize cliques, first layer */
        for (int i = 0; i < 100; i++) {
            int f1 = rand()%2048;
            int f2 = f1;

            while (f2 != f1) {
                f2 = rand() % 2048;
            }

            for (int j = 0; j < 100; j++) {
                QSet<int> clique;
                clique.insert(f1);
                clique.insert(f2);

                while (clique.size() < 8) {
                    clique.insert(rand()%2048);
                }
                cliques.push_back(clique);
            }
        }

        /* initialize cliques, second layer */
        for (int i = 0; i < cliques.size(); i++) {
            QSet<int> clique2;
            for (int j = 0; j < 8; j++) {
                clique2.insert(rand()%256 + 256*j);
            }

            foreach(int fanal, cliques[i]) {
                foreach(int fanal2, clique2) {
                    connections[fanal][fanal2] = reverseConnections[fanal2][fanal] = true;
                }
            }

            foreach(int fanal, clique2) {
                foreach(int fanal2, clique2) {
                    interConnectionsLayer2[fanal][fanal2] = true;
                }
            }

            cliques2.push_back(clique2);
        }

        //test cliques
        for (int test = 0; test < 200; test++) {
            nbTests ++;

            int testIndex = rand() % cliques.size();
            auto clique = cliques[testIndex];

            QMap<int, int> layer2;
            QList<QSet<int>> clusters;
            for (int i = 0; i < 8; i++) {clusters.push_back(QSet<int>());}

            foreach (int fanalO, clique) {
                for (int i = 0; i < 2048; i++) {
                    if (connections[fanalO][i]) {
                        layer2[i]++;
                        clusters[i/256].insert(i);
                    }
                }
            }

            auto winnerTakeAll = [&] {
                for (int clusterIndex = 0; clusterIndex < 8; clusterIndex++) {
                    QSet<int> cluster = clusters[clusterIndex];

                    int maxStr = 0;
                    foreach (int fanal, cluster) {
                        if (layer2[fanal] > maxStr) {
                            maxStr = layer2[fanal];
                        }
                    }

                    foreach (int fanal, cluster) {
                        if (layer2[fanal] < maxStr) {
                            layer2.remove(fanal);
                            clusters[clusterIndex].remove(fanal);
                        }
                    }
                }
            };

            winnerTakeAll();

            //now iterate 8 times on Second layer to find final clique
            for (int i = 0; i < 8; i++) {
                QMap<int, int> newLayer2;

                //propagate
                foreach (int fanal, layer2) {
                    for (int j = 0; j < 2048; j++) {
                        if (interConnectionsLayer2[fanal][j]) {
                            newLayer2[j] ++;
                        }
                    }
                }

                layer2.swap(newLayer2);

                //winner take all
                winnerTakeAll();
            }

            //test
            auto clique2 = cliques2[testIndex];

            foreach(int fanal, clique2) {
                if (!layer2.contains(fanal)) {
                    nbErrors++;
                }
            }
        }

        cout << "Error rate: " << (double(nbErrors)/nbTests) << endl;
    }

    return 0;
}
