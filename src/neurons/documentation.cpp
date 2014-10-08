#include <iostream>
#include "documentation.h"

using namespace std;

Documentation::Documentation()
{
}

void Documentation::print()
{
    cout << "This program is a simulation of the activity of the neurons in the brain." << endl;
    cout << "This is based on several hypotheses:" << endl;
    cout << "- There are groups of 'Fanals' (combination of 100 local neurons) that form 'Clusters'" << endl;
    cout << "- In a cluster, only 1 Fanal is active at a time" << endl;
    cout << "- An infon (unit of information) is represented by a group of fully interconnected fanals" << endl;
    cout << "- When fanals are active at the same time, they form connections\n" << endl;
    cout << "This is only a very basic model for now.\n" << endl;

    cout << "Use !exit to quit, !i <word> to add a word, and ?word to test a word" << endl;
}
