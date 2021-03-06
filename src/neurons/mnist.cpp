#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <iostream>
#include "graphics/mnistgraphics.h"
#include "graphics/neuronsgrid.h"
#include "mnist.h"
#include "easycliquenetwork.h"

using namespace std;

Mnist::Mnist()
{
}

void Mnist::load()
{
    QFile f("train-images.idx3-ubyte");
    f.open(QFile::ReadOnly);

    if (f.error()) {
        cout << "Error while loading MNIST database file " << f.fileName().toStdString() << endl;
        throw;
    }

    auto getInt = [&]() {
        unsigned char x[4];
        f.read((char*)x, 4);

        return x[3] + (x[2]<<8) + (x[1]<<16) + (x[0] << 24);
    };

    if (getInt() != 2051) {
        cout << "Error while loading MNIST database file, wrong magic number. " << endl;
        throw;
    }

    int nbImages = getInt();
    nbRows = getInt();
    nbCols = getInt();

    rawImages.resize(nbImages);
    for (int i = 0; i < nbImages; i++) {
        rawImages[i] = f.read(nbRows*nbCols);
    }

    //cout << "MNIST database loaded" << endl;
}

const QByteArray &Mnist::getImage(int index)
{
    if (images.contains(index)) {
        return images[index];
    }

    QByteArray rawImage = rawImages[index];
    QByteArray image;

//    for (int i = 0; i < nbRows; i ++) {
//        for (int j = 0; j < nbCols; j++) {
//            cout << ("00"+QString::number((int) (quint8)rawImage[i*nbCols+j])).right(3).toStdString() << "|";
//        }
//        cout << endl;
//    }

    for (int i = 0; i < nbRows; i += 4) {
        for (int j = 0; j < nbCols; j += 4) {
            int finalValue = 0;

            auto reducePixel = [&](int i, int j) {
                return (quint8(rawImage[i*nbCols+j]) + quint8(rawImage[(i+1)*nbCols+j]) + quint8(rawImage[i*nbCols+(j+1)]) + quint8(rawImage[(i+1)*nbCols+(j+1)]))/4;
            };

            int val;
            val = reducePixel(i, j);
            finalValue+= val >= 120;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i, j+2);
            finalValue+= val >= 120;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i+2, j);
            finalValue+= val >= 120;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i+2, j+2);
            finalValue+= val >= 120;
            finalValue+= val > 160;

            image.push_back(finalValue);
        }
    }

//    QList<int> values;
//    for (int i = 0; i < image.size(); i++) {
//        values.push_back(i*81+image[i]);
//    }

//    MnistGraphics graphics;
//    graphics.run(QList<QVector<int>>() << values.toVector());

    images[index] = image;

    return images[index];
}

double Mnist::test(TestType testType, int nbImages, int nbTests)
{
    EasyCliqueNetwork network;
    network.setSize(nbRows/4*nbCols/4, 3*3*3*3);

    QVector<QByteArray> cliques;
    std::uniform_int_distribution<> imageDist(0, rawImages.size()-1);
    std::uniform_int_distribution<> cliqueDist(0, nbImages-1);

    for(int i = 0; i < nbImages; i++) {
        int index = imageDist(randg());

        network.addClique(getImage(index));
        cliques.push_back(getImage(index));
    }

    if (testType == BlurTest) {
        //Remove white pixels, which are more trouble than they're worth
        network.removeConnectionsToFanals(0);
    }

    bool debug = false;
    //debug = true;

    if (debug) {
        network.activateDebug();
    }

    //cout << "testing..." << endl;
    int success = 0;
    for (int i = 0; i < nbTests; i++) {
        const auto &clique = cliques[cliqueDist(randg())];
        if (testType == EraseTest) {
            network.setupCliqueErased(clique, clique.size()/4);
            network.removeFanals(0);
        } else if (testType == InsertTest){
            network.setupClique(clique);
            network.removeFanals(0);
            network.insertFanals(0.25);
            network.losersTakeOut(4);
            network.fillRemaining(0);
        } else if (testType == BlurTest) {
            network.setupClique(clique);
            network.removeFanals(0);
            network.blurClique([](int x, int y) {
                int d1 = x%3 - y%3;
                x/= 3;
                y/= 3;
                int d2 = x%3 - y%3;
                x/= 3;
                y/= 3;
                int d3 = x%3 - y%3;
                x/= 3;
                y/= 3;
                int d4 = x%3 - y%3;

                if (d1*d1 + d2*d2 + d3*d3 + d4*d4 > 2 || x==0 || y == 0) {
                    return false;
                }
                return true;
            });
            network.fixGuide();
            network.fillRemaining(0);
        } else if (testType == ErrorTest) {
            network.setupClique(clique);
            network.removeFanals(0);
            network.errorClique(0.25);
            //network.losersTakeOut(3);
            network.fillRemaining(0);
        }

        network.iterate(8, 0, testType == BlurTest);

        if (testType == BlurTest) {
            network.fillRemaining(0);
        }

        if (network.matchClique(clique)) {
            success ++;
        } else if (debug) {
            QList<int> values;
            for (int i = 0; i < clique.size(); i++) {
                values.push_back(i*81+clique[i]);
            }

            MnistGraphics graphics;
            graphics.setExpected(values);
            graphics.run(network.getDebugStates());
        }
    }

    return 1.d - double(success)/nbTests;
}
