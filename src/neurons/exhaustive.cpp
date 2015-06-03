#include <vector>
#include <algorithm>
#include <iostream>
#include "utils.h"
#include "exhaustive.h"

using namespace std;

Exhaustive::Exhaustive()
{
}

void Exhaustive::run(int start, int end, int interval)
{
    (void) start;
    (void) end;
    (void) interval;
    int d = 2;
    int n = 64; // length of vector
    float p = 0.1; // Probability of flipping a bit
    int k = 32;

    vector<vector<int> > mat(k, vector<int>(n, 0));

    vector<int> onesAndZeros(k);
    fill(onesAndZeros.begin(), onesAndZeros.begin()+d, 1);

    for (int i = 0; i < k; i++) {
        shuffle(onesAndZeros.begin(), onesAndZeros.end(), randg());
        mat[i][i] = 1;
        for (int j = 0; j < k; j++) {
            mat[j][i+k] = onesAndZeros[j];
        }
    }

    //mat is now the generator matrix of the linear code.

    auto generateVector = [&]() {
        std::vector<int> ret(n);
        std::vector<int> source(k);
        std::uniform_int_distribution<> dist(0, 1);
        for (int i = 0; i < k; i++) {
            source[i] = dist(randg());
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < k; j++) {
                ret[i] += source[j] * mat[j][i];
            }
            ret[i] %= 2;
        }

        return ret;
    };

    auto noisy = [&](vector<int> &&v) {
        vector<int> ret(v);

        uniform_real_distribution<> dist(0, 1);
        for (int &i : ret) {
            i = dist(randg()) < p ? !i : i;
        }
        return ret;
    };

    auto cdot = [](const vector<int> v1, const vector<int> v2) {
        int res = 0;

        for (unsigned i = 0; i < v1.size(); i++) {
            res += v1[i] * v2[i];
        }

        return res % 2;
    };

    vector<vector<int>> noisyVectors;
    for (int n = start; n > 0; n--) {
        noisyVectors.emplace_back(std::move(noisy(std::move(generateVector()))));
    }

    int correct = 0;
    for (int i = 0; i < n-k; i++) {
        vector<int> toTest(k, 0);
        fill(toTest.end()-d, toTest.end(), 1);
        while ((signed)toTest.size() < n) {
            toTest.emplace_back(0);
        }
        toTest[k + i] = 1;
        int bestScore = 0;
        vector<int> bestVector;
        do {
            int score = 0;

            for (auto &&v : noisyVectors) {
                score += 1-cdot(v, toTest);
            }

            if (score > bestScore) {
                bestScore = score;
                bestVector = toTest;
                //cout << score << endl;
            }
        } while (std::next_permutation(toTest.begin(), toTest.begin()+k));

        // auto printVector = [](const vector<int> &v){
        //     for (int x : v) {
        //         cout << x << " ";
        //     }
        //     cout << endl;
        // };
        vector<int> expectedVector;

        for (int j = 0; j < k; j++) {
            expectedVector.emplace_back(mat[j][i+k]);
        }
        expectedVector.resize(n);
        expectedVector[k + i] = 1;

        // if (expectedVector != bestVector) {
        //     cout << "Error" << endl;
        //     printVector(bestVector);
        //     printVector(expectedVector);
        // } else {
        //     cout << "Success" << endl;
        // }
        correct += expectedVector == bestVector;
    }

    cout << float(correct) / (n-k) << endl;
}
