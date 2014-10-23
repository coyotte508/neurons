#include <random>
#include <algorithm>
#include <iostream>

#include "macros.h"
#include "utils.h"
#include "hopfield.h"

using namespace std;

static std::uniform_int_distribution<> dist(0, 1);

Hopfield::Hopfield(int size) : size(size)
{
    weights.resize(size);

    for (vector<int> &weightLine : weights) {
        weightLine.resize(size);
    }
}

void Hopfield::learnMessages(int N)
{
    for (int i = 0; i < N; i++) {
        debug(cout << "learning " << i << endl;)

        /* Create a random vector of 0s and 1s */
        vector<int> message(size);
        std::generate(message.begin(), message.end(), [] {return dist(randg());});

        //Not checking if the vector is already there, which is correct when size >> 1

        /* Training network for the new message */
        for (int l = 0; l < size; l++) {
            for (int c = 0; c < l; c++) {
                weights[l][c] = weights[c][l] = weights[c][l] + (2*message[l]-1)*(2*message[c]-1);
            }
        }

        messages.push_back(std::move(message));
    }
}

double Hopfield::testMessages(bool synapticNoise)
{
    int total(0), successes(0);

    vector<int> order(size);
    for (int i = 0; i < size; i++) order[i] = i;

    for (const vector<int>&  _message : messages) {
        vector<int> message = _message;

        //Add a noise on 1/4 of the message
        std::generate(message.begin(), message.begin()+size/4, [] {return 0;/*dist(randg());*/});

        bool changed;
        int counter = 1000;

        do {
            changed = false;
            std::random_shuffle(order.begin(), order.end(), [](int i) {return std::uniform_int_distribution<>(0, i-1)(randg());});

            for (auto it = order.begin(); it != order.end(); ++it) {
                int index = *it;

                int sum = 0;
                const auto &weightLine = weights[index];

                for (int i = 0; i < size; i++) {
                    if (i == index || message[i] == 0) continue;

                    if (!synapticNoise) {
                        sum += weightLine[i];
                    } else {
                        std::binomial_distribution<int> distribution(10, 0.5);

                        sum += weightLine[i] * distribution(randg());
                    }
                }

                int newVal = sum >= 0;

                if (newVal != message[index]) {
                    message[index] = newVal;
                    changed = true;
                }
            }
        } while (changed && --counter > 0);

        debug(cout << counter << endl;)

        successes += message == _message;
        total += 1;

        if (total >= 2000) {
            break;
        }
    }

    return double(total-successes)/total;
}
