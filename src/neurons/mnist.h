#ifndef MNIST_H
#define MNIST_H

#include <QVector>
#include <QByteArray>
#include <QMap>

class Mnist
{
public:
    Mnist();

    void load();

    const QByteArray &getImage(int index);

    void test(int nbImages);
private:
    QVector<QByteArray> rawImages;
    QMap<int, QByteArray> images;

    int nbRows, nbCols;
};

#endif // MNIST_H
