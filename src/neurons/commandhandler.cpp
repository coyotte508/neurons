#include <iostream>

#include "macros.h"
#include "jstring.h"
#include "macrocluster.h"
#include "documentation.h"
#include "commandhandler.h"

using namespace std;

typedef MacroCluster::Layer Layer;

namespace std
{
    template<>
    struct hash<std::unordered_set<Fanal*>>
    {
        typedef std::unordered_set<Fanal*> argument_type;
        typedef std::size_t result_type;

        result_type operator()(argument_type const& set) const
        {
            result_type seed = 0;
            /* Order independent hashing */
            for (const auto& elem : set) {
                seed ^= reinterpret_cast<size_t>(elem);;
            }
            return seed;
        }
    };
}


CommandHandler::CommandHandler() : silent(false)
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

    commands["simul1"] = [this](const jstring &s) {
        if (!silent) cout << "Simulating random clique presence depending on density" << endl;

        auto args = s.split(' ');

        if (args.size() < 3) {
            cout << "usage: !simul1 [nbclusters] [nbfanals] [density]" << endl;
            return;
        }

        int nbClusters = args[0].toInt();
        int fanalsPerCluster = args[1].toInt();
        double density = args[2].toDouble();

        MacroCluster mc({Layer(nbClusters, fanalsPerCluster)});

        Cluster *c = *mc.bottomLevel().begin();

        if (!silent) cout << "Learning cliques..." << endl;
        int count = 0;
        while (mc.density() < density) {
            count += c->learnRandomClique();
        }

        if (!silent) cout << "added " << count << " new cliques for density of " << mc.density() << endl;

        int nbLearned = 0, i = 0;
        constexpr int nbIter = 10*1000*1000;

        if (!silent) cout << "Testing cliques..." << endl;
        for (i = 0; i < nbIter && nbLearned < 1000; i++) {
            nbLearned += c->testRandomClique();
        }

        if (!silent) cout << nbLearned << "/" << i << endl;

        if (!silent) cout << "Probability of random clique with density " << mc.density() << ": " << (double(nbLearned)/i) << endl;
        if (silent) cout << (double(nbLearned)/i) << endl;
    };

    commands["simul2"] = [this](const jstring &s) {
        if (!silent) cout << "Simulating ratio of unlearnt cliques depending on density" << endl;

        auto args = s.split(' ');

        if (args.size() < 3) {
            cout << "usage: !simul2 [nbclusters] [nbfanals] [density]" << endl;
            return;
        }

        int nbClusters = args[0].toInt();
        int fanalsPerCluster = args[1].toInt();
        double density = args[2].toDouble();

        MacroCluster mc({Layer(nbClusters, fanalsPerCluster)});

        Cluster *c = *mc.bottomLevel().begin();

        if (!silent) cout << "Learning cliques..." << endl;
        std::unordered_set<std::unordered_set<Fanal*>> cliques;

        while (mc.density() < density) {
            auto clique = c->getRandomClique();
            if (!Fanal::interlinked(clique)) {
                Fanal::interlink(clique);
                cliques.insert(clique);
            }
        }

        if (!silent) cout << "added " << cliques.size() << " new cliques for density of " << mc.density() << endl;

        int nbRetrieved = 0, nbLearned = 0, i = 0;
        constexpr int nbIter = 10*1000*1000;

        if (!silent) cout << "Testing cliques..." << endl;
        for (i = 0; i < nbIter && nbRetrieved < 10000; i++) {
            auto clique = c->getRandomClique();
            if (Fanal::interlinked(clique)) {
                nbRetrieved += 1;
                nbLearned += cliques.find(clique) != cliques.end();
            }
        }

        if (!silent) cout << nbRetrieved << "/" << i << endl;
        if (!silent) cout << nbLearned << "/" << i << endl;

        double ratio;
        if (nbRetrieved == 0) {
            ratio = 0;
        } else if (nbLearned == 0) {
            ratio = nbRetrieved;
        } else {
            ratio = double(nbRetrieved-nbLearned)/nbLearned;
        }

        if (!silent) cout << "Ratio of retrieved unlearnt/learnt messages for density " << mc.density() << ": " << ratio << endl;
        if (silent) cout << ratio << endl;
    };
}

void CommandHandler::analyzeOptions(int argc, char **argv)
{
    jstring command = "!";

    for (int i = 0; i < argc; i++) {
        if (silent) {
            command += argv[i];
            command.push_back(' ');
        }
        if (strcasecmp(argv[i], "-help") == 0 || strcasecmp(argv[i], "--help") == 0) {
            Documentation::print();
        } else if (strcmp(argv[i], "-c") == 0) {
            silent = true;
        }
    }

    if (!silent) {
        cout << "Neuron simulator v0.0.1!" << endl;
    } else {
        analyzeCommand(command);
        throw QuitException();
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
        cout << "unrecognized command: " << instruction << endl;
        return;
    }

    if (pos == std::string::npos) {
        commands[instruction](jstring());
    } else {
        commands[instruction](s.substr(pos+1));
    }
}
