#include <random>
#include <iostream>
#include <set>
#include <QCoreApplication>
#include <QSet>
#include <QMap>
#include <QVector>
#include <cassert>

using namespace std;

default_random_engine generator;

class SparseNetwork {
public:
    int nbClusters = 100;
    int nbFanals = 64;
    int cliqueSize = 12;
    int memoryEffect = 1;

    QMap<int, QSet<int>> interConnections;

    void addMessages(int n) {
        uniform_int_distribution<> cdist(0, nbClusters-1);
        uniform_int_distribution<> fdist(0, nbFanals-1);

        while (n--) {
            QSet<int> clusters;
            QSet<int> message;

            while (clusters.size() < cliqueSize) {
                clusters.insert(cdist(generator));
            }

            for (int c : clusters) {
                message.insert(toFanal(c, fdist(generator)));
            }

            messages.push_back(message);
        }

        for (const QSet<int> &message : messages) {
            for (int f : message) {
                for (int f2: message) {
                    interConnections[f].insert(f2);
                }
            }
        }
    }

    bool testRandomMessageBlur(int delta=1, int its=1) {
        uniform_int_distribution<> mdist(0, messages.size()-1);
        uniform_int_distribution<> fdist(0, nbFanals-1);
        const auto& message = messages[mdist(generator)];
        QSet<int> messageMod = message;

        //Add blur by adding 2*delta similar fanals
        for (int f : message) {
            for (int i = 0; i < 2*delta; i++) {
                int x;
                while (messageMod.contains(x = toFanal(toCluster(f), fdist(generator)))) {
                }
                messageMod.insert(x);
            }
        }

        QSet<int> res = messageMod;
        while (its--) {
            res = iterate(res);
        };
        return message == res;
    }

    bool testRandomMessageErrors(int errors=3, int its  = 1) {
        uniform_int_distribution<> mdist(0, messages.size()-1);
        uniform_int_distribution<> fdist(0, nbFanals-1);
        const auto& message = messages[mdist(generator)];
        auto messageMod = message.toList();
        shuffle(messageMod.begin(), messageMod.end(), generator);

        //Add blur by adding 2*delta similar fanals
        for (int i = 0; i < errors; i++) {
            int f = messageMod[i];
            int x;
            while (messageMod.contains(x = toFanal(toCluster(f), fdist(generator)))) {
            }
            messageMod.push_back(x);
        }
        for (int i = 0; i < errors; i++) {
            messageMod.pop_front();
        }

        QSet<int> res = messageMod.toSet();
        while (its--) {
            res = iterate(res);
        };
        return message == res;
    }


    bool testRandomMessageErase(int erase=3, int its = 1) {
        uniform_int_distribution<> mdist(0, messages.size()-1);
        const auto& message = messages[mdist(generator)];
        auto messageMod = message.toList();
        shuffle(messageMod.begin(), messageMod.end(), generator);

        for (int i = 0; i < erase; i++) {
            messageMod.pop_back();
        }

        QSet<int> res = messageMod.toSet();
        while (its--) {
            res = iterate(res);
        };
        return message == res;
    }

    bool testRandomMessageIntr(int intr=6, int its=1) {
        uniform_int_distribution<> mdist(0, messages.size()-1);
        uniform_int_distribution<> fdist(0, nbFanals-1);
        uniform_int_distribution<> cdist(0, nbClusters-1);
        const auto& message = messages[mdist(generator)];
        auto messageMod = message.toList();
        QSet<int> clusters;
        for (int f : message) { clusters << toCluster(f);}

        for (int i = 0; i < intr; i++) {
            int c;
            while (clusters.contains(c = cdist(generator))) {
            }
            clusters.insert(c);
            messageMod.push_back(toFanal(c, fdist(generator)));
        }

        QSet<int> res = messageMod.toSet();
        while (its--) {
            res = iterate(res);
        };
        return message == res;
    }

    template <class T>
    QSet<int> iterate(T messageMod) {
        QMap<int, int> scores;

        QMap<int, QSet<int>> clusterContrib;
        //propagate
        for (int f: messageMod) {
            for (int f2 : interConnections[f]) {
                if (clusterContrib[toCluster(f)].contains(f2)) {
                    continue;
                }
                clusterContrib[toCluster(f)].insert(f2);
                scores[f2]++;
            }
            scores[f] += memoryEffect-1;
        }

        QMultiMap<int, int> strengths;
        //get best
        for (auto it = scores.begin(); it != scores.end(); ++it) {
            if (strengths.size() < cliqueSize) {
                strengths.insertMulti(it.value(), it.key());
                continue;
            }
            int minStrength = strengths.begin().key();
            if (it.value() < minStrength) {
                continue;
            }
            strengths.insertMulti(it.value(), it.key());

            if (strengths.size() > cliqueSize) {
                if (strengths.size() - strengths.count(strengths.begin().key()) >= cliqueSize) {
                    strengths.remove(strengths.begin().key());
                }
            }
        }

        return strengths.values().toSet();
    }

    int toCluster(int f) {
        return f / nbFanals;
    }

    int toFanal(int cluster, int fanal) {
        return fanal + cluster*nbFanals;
    }

    QVector<QSet<int> > messages;
};

int main(int argc, char **argv)
{
    SparseNetwork n;

    const char *networkDesc;
    int type, delta, its, nmessages;
    bool silent;

    if (argc >= 6) {
        silent = true;
        networkDesc = argv[1];
        type = argv[2][0];
        nmessages = atoi(argv[3]);
        delta = atoi(argv[4]);
        its = atoi(argv[5]);
    } else {
        //cout<< "Not enough arguments. " << endl; exit(0);
        silent = false;
        networkDesc = "cb";
        type = 'b';
        delta = 3;
        nmessages = 100000;
        its = 10;
    }

    if (networkDesc[0] == 'c' && networkDesc[1] == 's') {
        n.cliqueSize = 8;
        n.nbClusters = 8;
        n.nbFanals = 256;
    } else if (networkDesc[0] == 'w' && networkDesc[1] == 's') {
        n.cliqueSize = 8;
        n.nbClusters = 8*256;
        n.nbFanals = 1;
    } else if (networkDesc[0] == 'w' && networkDesc[1] == 'b') {
        n.nbClusters = 6400;
        n.nbFanals = 1;
    }

    //blur in a willshaw network is the same as insertions
    if (type == 'b' && networkDesc[0] == 'w') {
        type = 'i';
        delta = (2*delta*n.cliqueSize);
    }

    n.addMessages(nmessages);

    int ntests = 400;

    if (!silent) {
        cout << n.messages.size() << endl;
    }

    int successes = 0;
    if (type == 'b') {
        n.memoryEffect = 1000;
        for (int i = 0; i < ntests; i++) {
            if (!silent && i % 10 == 0) {
                cout << i << endl;
            }
            successes += n.testRandomMessageBlur(delta, its);
        }
    }
    if (type == 'e') {
        n.memoryEffect = 1000;
        for (int i = 0; i < ntests; i++) {
            successes += n.testRandomMessageErase(delta, its);
        }
    }
    if (type == 'r') {
        for (int i = 0; i < ntests; i++) {
            successes += n.testRandomMessageErrors(delta, its);
        }
    }
    if (type == 'i') {
        n.memoryEffect = 1000;
        for (int i = 0; i < ntests; i++) {
            successes += n.testRandomMessageIntr(delta, its);
        }
    }

    cout << (ntests-successes)*1.0/ntests << endl;
}
