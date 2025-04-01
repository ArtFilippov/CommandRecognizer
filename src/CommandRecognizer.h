#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <functional>

#include "CommandRelated.h"

class Pattern {
    int len{1};
    uint8_t adr;
    std::string name;
    std::function<bool(std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator)> patternMatcher;

  public:
    Pattern(std::string name, uint8_t adr, int len,
            std::function<bool(std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator)> patternMatcher);

    const std::string &getName() const { return name; }

    int getLen() const { return len; };

    uint8_t getAdr() const { return adr; }

    bool operator()(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end);
};

class CommandRecognizer {
  private:
    std::vector<uint8_t> buffer;
    int commandStartPosition{0};

    std::unordered_map<uint8_t, std::vector<Pattern>>
        patternMatchers; // <address, vector of patterns sorted by desc length>
    std::vector<Pattern>::iterator currentPattern;
    std::string currentPatternName;
    uint8_t currentPatternAdr;
    int currentPatternLength{0};

    std::queue<CommandView> recognized;

  public:
    static const int MAX_BUFFER_SIZE = 256;

    bool getCommand(CommandView &command);

    CommandRecognizer &operator<<(Buffer &message);

    void addPattern(Pattern &&pattern);

  private:
    void resetBuffer();

    bool startNewPattern(std::vector<uint8_t>::iterator &begin, std::vector<uint8_t>::iterator &end);

    bool checkBuffer();

    void gotoNextPattern();

    void startNewPatternFromBuffer();
};
