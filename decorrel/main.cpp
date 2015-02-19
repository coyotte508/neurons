#include <QCoreApplication>

#include <QSet>
#include <QMap>
#include <random>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    srand(time(NULL));

    int nbTests(0);
    int nbCodeErrors(0), nbCodeErrors2(0);
    int nbErrors(0);

    for (int network = 0; network < 200; network++) {
        cout << "Network " << (network + 1) << endl;

        QMap<int, QSet<int>> connections;
        QMap<int, QSet<int>> reverseConnections;
        QMap<int, QSet<int>> interConnectionsLayer2;
        QMap<int, QSet<int>> inhibit;

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
                    connections[fanal].insert(fanal2);
                    reverseConnections[fanal2].insert(fanal);
                }
            }

            foreach(int fanal, clique2) {
                foreach(int fanal2, clique2) {
                    interConnectionsLayer2[fanal].insert(fanal2);
                }
            }

            cliques2.push_back(clique2);
        }

        cout << "network initialized" << endl;

        //train cliques
        for (int cliqueIndex = 0; cliqueIndex < cliques2.size(); cliqueIndex++) {
            //transfer from layer 2 to layer 1
            QMap<int, int> layer;
            auto clique2 = cliques2[cliqueIndex];
            foreach(int fanal2, clique2) {
                foreach (int fanal, reverseConnections[fanal2]) {
                    layer[fanal]++;
                }
            }

            foreach(int fanal, layer.keys()) {
                if (layer[fanal] < 8) {
                    layer.remove(fanal);
                }
            }

            QSet<int> finalClique = layer.keys().toSet();
            QSet<int> expectedClique = cliques[cliqueIndex];

            for (int fanal: finalClique) {
                if (!expectedClique.contains(fanal)) {
                    for (int fanal2: clique2) {
                        inhibit[fanal2].insert(fanal);
                    }
                }
            }
        }

        cout << "network trained" << endl;

        //test cliques
        for (int test = 0; test < 200; test++) {
            //cout << "testing " << test << endl;
            nbTests ++;

            int testIndex = rand() % cliques.size();
            auto clique = cliques[testIndex];

            QMap<int, int> layer2;
            QList<QSet<int>> clusters;
            for (int i = 0; i < 8; i++) {clusters.push_back(QSet<int>());}

            //transfer from layer 1 to layer 2
            QList<int> cliqueV = clique.toList();
            for (int i = 0; i < 4; i++) {
                foreach (int fanal2, connections[cliqueV[i]]) {
                    layer2[fanal2]++;
                    clusters[fanal2/256].insert(fanal2);
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
                foreach (int fanal, layer2.keys()) {
                    foreach (int j, interConnectionsLayer2[fanal]) {
                        newLayer2[j] ++;
                        clusters[j/256].insert(j);
                    }
                }

                layer2 = newLayer2;

                //winner take all
                winnerTakeAll();
            }

            //test code error rate
            auto clique2 = cliques2[testIndex];
            auto resultLayer2 = layer2.keys().toSet();

            if (!resultLayer2.contains(clique2)) {
                nbCodeErrors++;
            }
            if (resultLayer2 != clique2) {
                nbCodeErrors2++;
            }

            //transfer from layer 2 to layer 1
            QMap<int, int> layer;
            QMap<int, int> inhibitValue;
            foreach(int fanal2, resultLayer2) {
                foreach (int fanal, reverseConnections[fanal2]) {
                    layer[fanal]++;

                    if (inhibit[fanal2].contains(fanal)) {
                        inhibitValue[fanal]++;
                    }
                }
            }

            foreach(int fanal, layer.keys()) {
                if (layer[fanal] < 8 || inhibitValue.value(fanal) >= 8) {
                    layer.remove(fanal);
                }
            }

            QSet<int> finalClique = layer.keys().toSet();

            if (finalClique != clique) {
                nbErrors ++;
            }
        }

        cout << "Strict Code Error rate: " << (double(nbCodeErrors2)/nbTests) << endl;
        cout << "Code Error rate: " << (double(nbCodeErrors)/nbTests) << endl;
        cout << "Final Error rate: " << (double(nbErrors)/nbTests) << endl;
    }

    char c;
    cin >> c;

    return 0;
}
