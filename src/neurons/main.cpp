#include <iostream>
#include "jstring.h"
#include "commandhandler.h"
#include "macrocluster.h"

using namespace std;

typedef MacroCluster::Layer Layer;

int _argc;
char **_argv;

int main(int argc, char **argv)
{
    _argc = argc;
    _argv = argv;

    CommandHandler handler;

    try {
        handler.analyzeOptions(argc, argv);
    } catch (CommandHandler::QuitException &) {
        return 0;
    }

    MacroCluster mc({Layer(9, 26), Layer(4, 256)});

    vector<Cluster*> inputs(mc.bottomLevel().begin(), mc.bottomLevel().end());

    auto inputWord = [&mc, &inputs] (const string& word){
        if (word.size() > inputs.size()) {
            return;
        }

        unordered_set<Fanal*> inputNeurons;

        /* use neurons of first & last clusters */
        for (unsigned i = 0; i < word.length(); i++) {
            auto i2 = i <= word.length()/2 ? i : inputs.size() - word.length() + i;

            inputNeurons.insert(inputs[i2]->fanal(word[i] - 'a'));
        }

        mc.flash(inputNeurons);
    };

    auto testWord = [&mc, &inputs] (const string &word) {
        unordered_set<Fanal*> inputNeurons, resultNeurons;

        /* use neurons of first & last clusters */
        for (unsigned i = 0; i < word.length(); i++) {
            auto i2 = i <= word.length()/2 ? i : inputs.size() - word.length() + i;

            if (word[i] >= 'a' && word[i] <= 'z') {
                inputNeurons.insert(inputs[i2]->fanal(word[i] - 'a'));
            }
        }

        cout << std::boolalpha << mc.testFlash(inputNeurons, &resultNeurons) << " " << Fanal::interlinked(resultNeurons) << " (";

        //Now to: find the word represented by the neurons
        string resultWord;
        for (Cluster *c : inputs) {
            for (Fanal *f : resultNeurons) {
                if (f->master() == c) {
                    for (int i = 0; i < 26; i++) {
                        if (c->fanal(i) == f) {
                            resultWord.push_back('a' + i);
                        }
                    }
                }
            }
        }

        cout << resultWord << ")" << endl;
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
            } catch (CommandHandler::MultipleLearnException &ml) {
                for (const jstring &word: ml.wordsToLearn) {
                    inputWord(word);
                }
                if (!handler.silent) {
                    cout << "learned dictionary" << endl;
                }
            } catch (CommandHandler::TestException& l) {
                testWord(l.wordToTest);
            }
        }
    }

    return 0;
}

