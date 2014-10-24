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

    int histo[11];
    std::fill_n(histo, 11, 0);
    std::fill_n(cumulBinomial, 11, 0);

    std::binomial_distribution<int> distribution(10, 0.5);
    for (int i = 0; i < 65536; i++) {
        histo[distribution(randg())] += 1;
    }

    for (int i = 0; i <= 10; i++) {
        for (int j = i; j <= 10; j++) {
            cumulBinomial[i] += histo[i];
        }
    }
}

void Hopfield::learnMessages(int N)
{
    for (int i = 0; i < N; i++) {
        debug(cout << "learning " << i << endl;)

        /* Create a random vector of 0s and 1s */
        vector<int> message(size);
        std::generate(message.begin(), message.end(), [] {return -1 + 2 * dist(randg());});

        //Not checking if the vector is already there, which is correct when size >> 1

        /* Training network for the new message */
        for (int l = 0; l < size; l++) {
            for (int c = 0; c < l; c++) {
                weights[l][c] = weights[c][l] = weights[c][l] + message[l]*message[c];
            }
        }

        messages.push_back(std::move(message));
    }
}

double Hopfield::testMessages(int nb, bool synapticNoise)
{
    int total(0), successes(0);

    vector<int> order(size);
    for (int i = 0; i < size; i++) order[i] = i;

    for (int cnt = 0; cnt < nb; cnt++) {
        int alea = std::uniform_int_distribution<>(0, messages.size()-1)(randg());
        const vector<int> &_message = messages[alea];
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

                if (!synapticNoise) {
                    for (int i = 0; i < size; i++) {
                        if (i == index) continue;

                        sum += weightLine[i] * message[i];
                    }
                } else {
                    for (int i = 0; i < size; i++) {
                        int num = (randg()()) & (65536-1);
                        int j;
                        for (j = 0; j < 10 && cumulBinomial[j] < num; j++) {
                        }
                        sum += weightLine[i] * j * message[i];
                    }
                }

                int newVal = -1 + 2 *(sum >= 0);

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

    cout << total << " " << successes << endl;
    return double(total-successes)/total;
}
