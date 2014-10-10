#include <iostream>

#include "macros.h"
#include "jstring.h"
#include "macrocluster.h"
#include "documentation.h"
#include "commandhandler.h"

using namespace std;

typedef MacroCluster::Layer Layer;

CommandHandler::CommandHandler()
{
    commands["help"] = [](const jstring &) {
        Documentation::print();
    };

    commands["quit"] = commands["exit"] = [](const jstring &) {
        throw QuitException();
    };

    commands["start"] = [](const jstring &) {
        throw StartException();
    };

    commands["i"] = [](const jstring &s) {
        throw LearnException(s.trim());
    };

    commands["test"] = [](const jstring &s) {
        debug(cout << "testing " << s << endl;)
        throw TestException(s.trim());
    };

    commands["simul1"] = [](const jstring &s) {
        cout << "Simulating  " << s << endl;

        MacroCluster mc({Layer(4, 512)});

        Cluster *c = *mc.bottomLevel().begin();

        cout << "Learning cliques..." << endl;
        int count = 0;
        while (mc.density() < 0.2) {
            count += c->learnRandomClique();
        }

        cout << "added " << count << " new cliques for density of " << mc.density() << endl;

        int nbLearned = 0, i = 0;
        constexpr int nbIter = 10*1000*1000;

        cout << "Testing cliques..." << endl;
        for (i = 0; i < nbIter && nbLearned < 1000; i++) {
            nbLearned += c->testRandomClique();
        }

        cout << nbLearned << "/" << i << endl;

        cout << "Probability of random clique with density " << mc.density() << ": " << (double(nbLearned)/i) << endl;
    };
}

void CommandHandler::analyzeOptions(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        if (strcasecmp(argv[i], "-help") == 0 || strcasecmp(argv[i], "--help") == 0) {
            Documentation::print();
        }
    }
}

void CommandHandler::analyzeCommand(const jstring& s)
{
    auto pos = s.find(' ');
    jstring instruction = s.substring(1, pos);

    if (s.starts_with("?")) {
        commands["test"](instruction);
        return;
    }

    if (commands.count(instruction) == 0) {
        cout << "unrecognized command!" << endl;
        return;
    }

    if (pos == std::string::npos) {
        commands[instruction](jstring());
    } else {
        commands[instruction](s.substr(pos+1));
    }
}
