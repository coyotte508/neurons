#include <iostream>
#include <fstream>

#include "graphics/neuronsgrid.h"
#include "macros.h"
#include "jstring.h"
#include "macrocluster.h"
#include "hopfield.h"
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
    commands["dict"] = [this](const jstring &) {
        learnDictionary();
    };

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
        debug(cout << "testing " << s << endl);
        throw TestException(s.trim());
    };

    commands["gui"] = [this](const jstring &) {
        startGui();
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

    commands["simul3"] = [this](const jstring &s) {
        if (!silent) cout << "Simulating error retrieval rate of partial messages" << endl;

        auto args = s.split(' ');

        if (args.size() < 4) {
            cout << "usage: !simul3 [nbclusters] [nbfanals] [nbknown] [nbmess] [?nbiter] [?binomial]" << endl;
            return;
        }

        int nbClusters = args[0].toInt();
        int fanalsPerCluster = args[1].toInt();
        int knownClusters = args[2].toInt();
        unsigned nbMessages = args[3].toInt();
        int nbIter = 1;

        if (args.size() > 4) {
            nbIter = args[4].toInt();
        }

        MacroCluster mc({Layer(nbClusters, fanalsPerCluster)});

        if (args.size() > 5 && args[5].toInt() == 1) {
            mc.setSynapses(10, 0.5f);
        }

        Cluster *c = *mc.bottomLevel().begin();

        if (!silent) cout << "Learning cliques..." << endl;
        std::unordered_set<std::unordered_set<Fanal*>> cliques;

        while (cliques.size() < nbMessages) {
            auto clique = c->getRandomClique();
            if (!Fanal::interlinked(clique)) {
                Fanal::interlink(clique);
                cliques.insert(clique);
            }
        }

        if (!silent) cout << "added " << cliques.size() << " new cliques for density of " << mc.density() << endl;

        int nbRetrieved = 0, nbInterlinked=0, nbInit=0, counter=0;

        if (!silent) cout << "Testing cliques..." << endl;
        for (const std::unordered_set<Fanal*> &clique : cliques) {
            counter ++;
            auto clique2 = clique;
            decltype(clique2) clique3;

            for (int i = 0; i < nbClusters-knownClusters; i++) {
                clique2.erase(clique2.begin());
            }

            nbInit += mc.testFlash(clique2, &clique3, nbIter);

            if (clique3 == clique) {
                nbRetrieved ++;
                nbInterlinked++;
            } else {
                if (Fanal::interlinked(clique3)) {
                    nbInterlinked++;
                }
            }

            if (counter >= 2000) {
                break;
            }
        }

        if (!silent) cout << nbRetrieved << "/" << counter << endl;
        if (!silent) cout << nbInit << "/" << counter << endl;
        if (!silent) cout << nbInterlinked << "/" << counter << endl;

        double errorRate = 1 - double(nbRetrieved)/counter;

        if (!silent) cout << "Error rate for size " << cliques.size() << ": " << errorRate << endl;
        if (silent) cout << errorRate << " " << mc.density() << endl;
    };

    commands["simul4"] = [this](const jstring &s) {
        simul4(s);
    };

    commands["hopfield"] = [this](const jstring &s) {
        if (!silent) cout << "Simulating error retrieval rate in hopfield network with 1/4 loss" << endl;

        auto args = s.split(' ');

        if (args.size() < 2) {
            cout << "usage: !hopfield [size] [nbmess]" << endl;
            return;
        }

        int size = args[0].toInt();
        unsigned nbMessages = args[1].toInt();

        int noise = 0;
        if (args.size() > 2) {
            noise = args[2].toInt();
        }

        double errorRate = 0;

        for (int i = 0; i < 30; i++) {
            Hopfield network(size);
            if (!silent) cout << "Learning messages... " << endl;
            network.learnMessages(nbMessages);

            if (!silent) cout << "Testing messages... " << endl;
            errorRate += network.testMessages(33, noise);
        }
        errorRate /= 30;

        if (silent) cout << errorRate << endl;
        if (!silent) cout << "Error rate: " << errorRate << endl;
    };

    commands["stabletest"] = [this](const jstring &s) {
        if (!silent) cout << "Simulating persistence of message" << endl;

        auto args = s.split(' ');

        if (args.size() < 4) {
            cout << "usage: !stabletest [nbclusters] [nbfanals] [msglength] [nbmess] [?nbiter]" << endl;
            return;
        }

        int nbClusters = args[0].toInt();
        int fanalsPerCluster = args[1].toInt();
        int clustersPerMessage = args[2].toInt();
        unsigned nbMessages = args[3].toInt();
        int nbIter = 1;

        if (args.size() > 4) {
            nbIter = args[4].toInt();
        }

        MacroCluster mc({Layer(nbClusters, fanalsPerCluster)});

        mc.setMemoryEffect(false);
        mc.setSynapses(10, 0.5f);

        int nbTimes = 1;

        if (args.size() > 5) {
            nbTimes = args[5].toInt();
        }

        mc.setCliqueSize(clustersPerMessage);
        mc.setConstantInput(false);

        Cluster *c = *mc.bottomLevel().begin();
        std::unordered_set<std::unordered_set<Fanal*>> cliques;

        for (int time = 1; time <= nbTimes; time++) {
            if (!silent) cout << "Learning cliques..." << endl;
            while (cliques.size() < nbMessages*time) {
                auto clique = c->getRandomClique(clustersPerMessage);
                if (!Fanal::interlinked(clique)) {
                    Fanal::interlink(clique);
                    cliques.insert(clique);
                }
            }

            if (!silent) cout << "Reached " << cliques.size() << " cliques for density of " << mc.density() << endl;

            int nbRetrieved = 0, nbInterlinked=0, nbInit=0, counter=0, nbIts = 0;

            if (!silent) cout << "Testing cliques..." << endl;
            for (const std::unordered_set<Fanal*> &clique : cliques) {
                counter ++;
                std::unordered_set<Fanal*> clique3;

                int its = mc.testFlash(clique, &clique3, nbIter);
                nbInit += its > 0;
                nbIts += its;

                if (clique3 == clique) {
                    nbRetrieved ++;
                    nbInterlinked++;
                } else {
                    if (Fanal::interlinked(clique3)) {
                        nbInterlinked++;
                    }
                }

                if (counter >= 2000) {
                    break;
                }
            }

            if (!silent) cout << nbRetrieved << "/" << counter << endl;
            if (!silent) cout << nbInit << "/" << counter << endl;
            if (!silent) cout << nbInterlinked << "/" << counter << endl;

            double errorRate = 1 - double(nbRetrieved)/counter;

            if (!silent) cout << "Error rate for size " << cliques.size() << ": " << errorRate << endl;
            if (silent) cout << errorRate << " " << mc.density() << endl;
        }
    };

    commands["iterate"] = commands["iter"] = [this](const jstring &) {
        NeuronsGrid grid;

        MacroCluster mc({Layer(8, 256)});
        mc.setSynapses(1, -1);
        //mc.setSpontaneousRelease(0.002);
        //mc.setMinimumExcitation(Fanal::defaultFlashStrength*3/5);
        mc.setCliqueSize(3);
        mc.setEpsilon(0.001);
        mc.setMu(10);

        std::vector<std::unordered_set<Fanal*> > cliques;
        for (int i = 0; i < 10 /* 1000*/; i++) {
            cliques.push_back(mc.getRandomClique(3));
        }

        //interlink sparsely
        //mc.interlink(0.1);

        //Train network
        for (int i = 0; i < 20000; i++) {
            if (i % 1000 == 0) {
                cout << "training round " << i << " ..." << endl;
            }

            for (unsigned j = 0; j < cliques.size(); j++) {
                //cout << "clique " << j << endl;
                mc.lightDown();
                mc.setInputs(cliques[j]);
                for (int k = 0; k < 5; k++) {
                    mc.iterate();
                }
            }
        }

        grid.setMacroCluster(&mc);
        grid.setCliques(cliques);
        grid.run();

        //throw QuitException();
    };

    commands["simul5"] = [this](const jstring &args){
        simul5(args);
    };
}

void CommandHandler::simul4(const jstring &s)
{
    if (!silent) cout << "Simulating error retrieval rate of partial messages in sparse network" << endl;

    auto args = s.split(' ');

    if (args.size() < 5) {
        cout << "usage: !simul4 [nbclusters] [nbfanals] [msglength] [nbknown] [nbmess] [?nbiter] [?proba] [nbTimes]" << endl;
        return;
    }

    int nbClusters = args[0].toInt();
    int fanalsPerCluster = args[1].toInt();
    int clustersPerMessage = args[2].toInt();
    int knownClusters = args[3].toInt();
    unsigned nbMessages = args[4].toInt();
    int nbIter = 1;

    if (args.size() > 5) {
        nbIter = args[5].toInt();
    }

    MacroCluster mc({Layer(nbClusters, fanalsPerCluster)});

    if (args.size() > 6 && args[6].toInt() > 0) {
        int val = args[6].toInt();

        if (val != 100) {
            mc.setMemoryEffect(false);
        }
        if (val == 1) {
            mc.setSynapses(10, 0.5f);
        } else {
            mc.setSynapses(10, double(val)/100);
        }
    }

    int nbTimes = 2000;

    if (args.size() > 7) {
        nbTimes = args[7].toInt();
    }

    mc.setCliqueSize(clustersPerMessage);

    if (args.size() > 8) {
        mc.setSpontaneousRelease(args[8].toDouble());
    }

    int successiveIters = 2;
    if (args.size() > 9) {
        successiveIters = args[9].toInt();
    }

    Cluster *c = *mc.bottomLevel().begin();
    std::unordered_set<std::unordered_set<Fanal*>> cliques;
    std::vector<std::unordered_set<Fanal*>> cliques_v;

    cliques_v.reserve(nbMessages);

    int time = 1;

    if (!silent) cout << "Learning cliques..." << endl;
    while (cliques.size() < nbMessages*time) {
        auto clique = c->getRandomClique(clustersPerMessage);
        if (cliques.count(clique) == 0) {
            Fanal::interlink(clique);
            cliques.insert(clique);
            cliques_v.push_back(clique);
        }
    }

    if (!silent) cout << "Reached " << cliques.size() << " cliques for density of " << mc.density() << endl;

    int nbRetrieved = 0, nbInterlinked=0, counter=0, nbIts=0;

    if (!silent) cout << "Testing cliques..." << endl;

    std::uniform_int_distribution<> cliquesDist(0, cliques_v.size() -1);
    while (counter < nbTimes) {
        counter ++;

        int cliqueIndex = cliquesDist(randg());
        //cout << cliqueIndex << endl;
        const auto &clique = cliques_v[cliqueIndex];
        std::unordered_set<Fanal*> clique2 = clique;
        decltype(clique2) clique3;

        for (int i = 0; i < clustersPerMessage-knownClusters; i++) {
            clique2.erase(clique2.begin());
        }

        auto its = mc.testFlash(clique2, &clique3, nbIter, successiveIters);
        nbIts += its;

        if (clique3 == clique) {
            nbRetrieved ++;
            nbInterlinked++;
        } else {
            if (Fanal::interlinked(clique3)) {
                nbInterlinked++;
            }
        }

        if (gui) {
            NeuronsGrid grid;
            grid.setMacroCluster(&mc);
            grid.setExpected(clique);
            grid.run();
        }

        if (!silent && counter % 100 == 0) {
            cout << counter << "..." << endl;
        }
    }

    if (!silent) cout << nbRetrieved << "/" << counter << endl;
    if (!silent) cout << nbInterlinked << "/" << counter << endl;
    if (!silent) cout << (double(nbIts)/counter) << " iterations" << endl;

    double errorRate = 1 - double(nbRetrieved)/counter;

    if (!silent) cout << "Error rate for size " << cliques.size() << ": " << errorRate << endl;
    if (silent) cout << errorRate << " " << mc.density() << " " << (double(nbIts)/counter) << endl;
}

void CommandHandler::simul5(const jstring &s)
{
    std::ofstream ofs;

    int nclusters, nfanals, increment, cliqueSize, erased;
    bool willshaw = false;
    bool spread = false;

    auto args = s.split(' ');

    if (args.size() > 0) {
        if (args[0].toInt() == 1) {
            willshaw = true;
        } else if (args[0].toInt() == 1) {
            spread = true;
        }
    }

    if (willshaw) {
        nclusters = 6400;
        nfanals = 1;
        increment = 6000;
        cliqueSize = 12;
        erased = 3;
        if (!silent) {
            ofs.open ("out-willshaw.txt", std::ofstream::out);
        }
    } else {
        nclusters = 8;
        nfanals = 256;
        increment = 1000;
        cliqueSize = 8;
        erased = 4;
        if (!silent) {
            char name[20];
            sprintf(name, "out-full-%d.txt", std::uniform_int_distribution<>(0, 100000)(randg()));
            ofs.open (name, std::ofstream::out);
        }
    }

    std::ostream & out = silent ? cout : ofs;

    MacroCluster mc({Layer(spread ? nclusters*nfanals : nclusters, spread ? 1 : nfanals)});
    MacroCluster mcref({Layer(nclusters, nfanals)});

    //mc.setMemoryEffect(false);

    std::unordered_map<Fanal*, Fanal*> corres;
    std::unordered_map<Fanal*, Fanal*> rcorres;

    int indexRef = 0;
    auto itref = mcref.bottomLevel().begin();
    for (Cluster *c : mc.bottomLevel()) {
        for (int i = 0; i < c->size(); i++) {
            corres[c->fanal(i)] = (*itref)->fanal(indexRef);
            rcorres[(*itref)->fanal(indexRef)] = c->fanal(i);

            indexRef ++;
            if (indexRef >= nfanals) {
                indexRef = 0;
                ++itref;
            }
        }
    }

    mc.setSynapses(1, -1);
    //mc.setSpontaneousRelease(0.002);
    //mc.setMinimumExcitation(Fanal::defaultFlashStrength*3/5);
    mc.setCliqueSize(cliqueSize);
    mcref.setCliqueSize(cliqueSize);
    //mc.setEpsilon(0.001);
    //mc.setMu(10);

    std::vector<std::unordered_set<Fanal*> > cliques;
    std::vector<std::unordered_set<Fanal*> > cliquesRef;

    for (int k = 0; k < 30; k++) {
        for (int i = 0; i < increment; i++) {
            auto cliqueRef = mcref.getRandomClique(cliqueSize);
            cliquesRef.push_back(cliqueRef);
            Clique clique;
            for (auto it = cliqueRef.begin(); it != cliqueRef.end(); ++it) {
                clique.insert(rcorres[*it]);
            }
            cliques.push_back(clique);
            Fanal::interlink(cliqueRef);
        }
        for (unsigned j = cliques.size()-increment; j < cliques.size(); j++) {
            if (j % 100 == 0) {
                if (!silent) cout << "Learning clique " << j << endl;
            }
            mc.lightDown();
            mc.setInputs(cliques[j]);
            for (int k = 0; k < 60; k++) {
                mc.iterate();
            }
        }
        int totalLinks (0);
        int totalLinksRef (0);

        for (Cluster *c: mc.bottomLevel()) {
            for (int i = 0; i < c->size(); i++) {
                Fanal *f = c->fanal(i);

                totalLinks += f->nbLinks(0.8);
                totalLinksRef += corres[f]->nbLinks();
            }
        }
        mc.lightDown();
        mcref.lightDown();

        int error = 0;
        int errorRef = 0;
        int total = 0;

        std::uniform_int_distribution<> cliquesDist(0, cliques.size() -1);

        for (int ctest = 0; ctest < 2000; ctest++) {
            int index = cliquesDist(randg());
            const auto &clique = cliques[index];
            const auto &cliqueRef = cliquesRef[index];
            std::unordered_set<Fanal*> clique2 = clique;
            std::unordered_set<Fanal*> clique2Ref;
            decltype(clique2) clique3, clique3Ref;

            for (int i = 0; i < erased; i++) {
                clique2.erase(clique2.begin());
            }
            for (auto it = clique2.begin(); it != clique2.end(); ++it) {
                clique2Ref.insert(corres[*it]);
            }
            mc.testFlash(clique2, &clique3, 100, 2);
            mcref.testFlash(clique2Ref, &clique3Ref, 100, 2);

            total ++;
            if (clique3 != clique) {
                error ++;
            }
            if (clique3Ref != cliqueRef) {
                errorRef ++;
            }
            //auto debug2 = std::set<Fanal*>(clique2.begin(), clique2.end());
            //auto debug3 = std::set<Fanal*>(clique3.begin(), clique3.end());
            //auto debug2ref = std::set<Fanal*>(clique2Ref.begin(), clique2Ref.end());
            //auto debug3ref = std::set<Fanal*>(clique3Ref.begin(), clique3Ref.end());
        }

        if (!silent) {
            cout << "Error rate: " << (double(error) / total) << endl;
            cout << "Reference error rate: " << (double(errorRef) / total) << endl;
            cout << "Correct links: " << (double(totalLinksRef) / totalLinks) << endl;
        }

        out << (double(error) / total) << " " << (double(errorRef) / total) /*<<
               " " << (double(totalLinksRef) / totalLinks)*/ << endl;
    }
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
        } else if (strcmp(argv[i], "-g") == 0) {
            gui = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            save = true;
        }
    }

    if (!silent) {
        cout << "Neuron simulator v0.0.2!" << endl;
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

void CommandHandler::learnDictionary()
{
    fstream in;
    in.open("/usr/share/dict/words", ios_base::in);

    jstring s;

    MultipleLearnException ex;

    int counter = 0;
    while (in >> s) {
        if (++ counter % 1000 == 0) {
            cout << "input word " << s << " " << counter << endl;
        }

        if (!s.isLowerAlphabetical()) {
            continue;
        }
        ex.wordsToLearn.push_back(s);
    }

    if (ex.wordsToLearn.size() > 0) {
        cout << "launching exception" << endl;
        throw ex;
    } else {
        cerr << "NO WORDS LEARNED!" << endl;
    }
}

void CommandHandler::startGui()
{
    MacroCluster mc({Layer(8,256)});
    NeuronsGrid grid;
    grid.setMacroCluster(&mc);
    grid.run();
}
