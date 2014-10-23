#ifndef HOPFIELD_H
#define HOPFIELD_H

#include <vector>

class Hopfield
{
public:
    Hopfield(int size);

    void learnMessages(int N);
    //Return error rate of the network
    double testMessages(bool synapticNoise = false);

private:
    std::vector<std::vector<int>> weights;
    std::vector<std::vector<int>> messages;

    int size;
};

#endif // HOPFIELD_H
