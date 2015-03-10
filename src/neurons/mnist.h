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
private:
    QVector<QByteArray> rawImages;
    QMap<int, QByteArray> images;
};

#endif // MNIST_H
