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

    QVector<QByteArray> rawImages;
    rawImages.resize(nbImages);
    for (int i = 0; i < nbImages; i++) {
        rawImages[i] = f.read(nbRows*nbCols);
    }

    qDebug() << "MNIST database loaded";
}
