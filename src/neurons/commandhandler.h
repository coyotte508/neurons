#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <unordered_map>
#include <functional>
#include <vector>

class jstring;

class CommandHandler
{
public:
    CommandHandler();

    void analyzeOptions(int argc, char **argv);
    void analyzeCommand(const jstring &s);

    void learnDictionary();
    void startGui();

    struct QuitException {};
    struct StartException {};
    struct LearnException {
        LearnException(const std::string &s):wordToLearn(s){}

        std::string wordToLearn;
    };
    struct MultipleLearnException {
        std::vector<std::string> wordsToLearn;
    };
    struct TestException {
        TestException(const std::string &s):wordToTest(s){}

        std::string wordToTest;
    };

    bool silent = false;
    bool gui = false;
    bool save = false;
private:
    std::unordered_map<std::string, std::function<void(const jstring&)>> commands;

    void simul4(const jstring&);
    void simul5(const jstring&);
    void sparseblur(const jstring&);
    void pb2(const jstring&);
    void exhaustive(const jstring&);
};

#endif // COMMANDHANDLER_H
