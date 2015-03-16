#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <iostream>
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

    for (int i = 0; i < nbRows; i += 4) {
        for (int j = 0; j < nbCols; j += 4) {
            int finalValue = 0;

            auto reducePixel = [&](int i, int j) {
                return (quint8(rawImage[i*nbCols+j]) + quint8(rawImage[(i+1)*nbCols+j]) + quint8(rawImage[i*nbCols+(j+1)]) + quint8(rawImage[(i+1)*nbCols+(j+1)]))/4;
            };

            int val;
            val = reducePixel(i, j);
            finalValue+= val >= 80;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i+2, j);
            finalValue+= val >= 80;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i, j+2);
            finalValue+= val >= 80;
            finalValue+= val > 160;
            finalValue *= 3;
            val = reducePixel(i+2, j+2);
            finalValue+= val >= 80;
            finalValue+= val > 160;

            image.push_back(finalValue);
        }
    }

    images[index] = image;

    return images[index];
}

double Mnist::test(int nbImages, int nbTests)
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

    //cout << "testing..." << endl;
    int success = 0;
    for (int i = 0; i < nbTests; i++) {
        if (network.testCliqueErased(cliques[cliqueDist(randg())], nbRows/4*nbCols/4/4)) {
            success++;
        }
    }

    return double(success)/nbTests;
}
