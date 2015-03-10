#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <iostream>
#include "mnist.h"

using namespace std;

Mnist::Mnist()
{
}

void Mnist::load()
{
    QFile f("train-images.idx3-ubyte");
    f.open(QFile::ReadOnly);

    if (f.error()) {
        qDebug() << "Error while loading MNIST database file " << f.fileName();
        throw;
    }

    auto getInt = [&]() {
        unsigned char x[4];
        f.read((char*)x, 4);

        return x[3] + (x[2]<<8) + (x[1]<<16) + (x[0] << 24);
    };

    if (getInt() != 2051) {
        qDebug() << "Error while loading MNIST database file, wrong magic number. ";
        throw;
    }

    int nbImages = getInt();
    int nbRows = getInt();
    int nbCols = getInt();

    rawImages.resize(nbImages);
    for (int i = 0; i < nbImages; i++) {
        rawImages[i] = f.read(nbRows*nbCols);
    }

    qDebug() << "MNIST database loaded";
}

const QByteArray &Mnist::getImage(int index)
{
    if (images.contains(index)) {
        return images[index];
    }

    QByteArray rawImage = rawImages[index];
    QByteArray image;

    for (int i = 0; i < 28; i += 4) {
        for (int j = 0; j < 28; j += 4) {
            int finalValue;

            finalValue+= quint8(rawImage[i*28+j]) >= 80;
            finalValue+= quint8(rawImage[i*28+j]) > 160;
            finalValue *= 3;
            finalValue+= quint8(rawImage[i*28+j+1]) >= 80;
            finalValue+= quint8(rawImage[i*28+j+1]) > 160;
            finalValue *= 3;
            finalValue+= quint8(rawImage[(i+1)*28+j]) >= 80;
            finalValue+= quint8(rawImage[(i+1)*28+j]) > 160;
            finalValue *= 3;
            finalValue+= quint8(rawImage[(i+1)*28+(j+1)]) >= 80;
            finalValue+= quint8(rawImage[(i+1)*28+(j+1)]) > 160;

            image.push_back(finalValue);
        }
    }

    images[index] = image;

    return images[index];
}
