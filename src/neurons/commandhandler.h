#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <unordered_map>
#include <functional>

class jstring;

class CommandHandler
{
public:
    CommandHandler();

    void analyzeOptions(int argc, char **argv);
    void analyzeCommand(const jstring &s);

    struct QuitException {};
    struct StartException {};
    struct LearnException {
        LearnException(const std::string &s):wordToLearn(s){}

        std::string wordToLearn;
    };
    struct TestException {
        TestException(const std::string &s):wordToTest(s){}

        std::string wordToTest;
    };

    bool silent = false;
private:
    std::unordered_map<std::string, std::function<void(const jstring&)>> commands;
};

#endif // COMMANDHANDLER_H
