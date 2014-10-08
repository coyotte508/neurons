#include <iostream>
#include "jstring.h"
#include "commandhandler.h"
#include "macrocluster.h"

using namespace std;

typedef MacroCluster::Layer Layer;

int main(int argc, char **argv)
{
    cout << "Neuron simulator v0.0.1!" << endl;

    CommandHandler handler;
    handler.analyzeOptions(argc, argv);

    MacroCluster mc({Layer(9, 26), Layer(4, 256)});

    vector<Cluster*> inputs(mc.bottomLevel().begin(), mc.bottomLevel().end());

    auto inputWord = [&mc, &inputs] (const string& word){
        unordered_set<Fanal*> inputNeurons;

        /* use neurons of first & last clusters */
        for (unsigned i = 0; i < word.length(); i++) {
            auto i2 = i <= word.length()/2 ? i : inputs.size() - word.length() + i;

            inputNeurons.insert(inputs[i2]->fanal('z' - word[i]));
        }

        mc.flash(inputNeurons);
    };

    auto testWord = [&mc, &inputs] (const string &word) {
        unordered_set<Fanal*> inputNeurons;

        /* use neurons of first & last clusters */
        for (unsigned i = 0; i < word.length(); i++) {
            auto i2 = i <= word.length()/2 ? i : inputs.size() - word.length() + i;

            if (word[i] >= 'a' && word[i] <= 'z') {
                inputNeurons.insert(inputs[i2]->fanal('z' - word[i]));
            }
        }

        return mc.testFlash(inputNeurons);
    };

    while (1) {
        jstring s;
        getline(cin, s);

        if (s.starts_with("!") || s.starts_with("?")) {
            try {
                handler.analyzeCommand(s);
            } catch (CommandHandler::QuitException&) {
                return 0;
            } catch (CommandHandler::StartException&) {
                break;
            } catch (CommandHandler::LearnException& l) {
                 inputWord(l.wordToLearn);
            } catch (CommandHandler::TestException& l) {
                cout << std::boolalpha << testWord(l.wordToTest) << endl;
            }
        }
    }

    return 0;
}

