#ifndef MNIST_H
#define MNIST_H

#include <QVector>
#include <QByteArray>
#include <QMap>

class Mnist
{
public:
    enum TestType {
        EraseTest, //d as in deletion
        BlurTest, //b
        ErrorTest, //e
        InsertTest //i
    };
public:
    Mnist();

    void load();

    const QByteArray &getImage(int index);

    double test(TestType testType, int nbImages, int nbTests);
private:
    QVector<QByteArray> rawImages;
    QMap<int, QByteArray> images;

    int nbRows, nbCols;
};

#endif // MNIST_H
