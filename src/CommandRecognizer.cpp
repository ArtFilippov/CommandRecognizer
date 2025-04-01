#include "CommandRecognizer.h"

Pattern::Pattern(std::string name, uint8_t adr, int len,
                 std::function<bool(std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator)> patternMatcher)
    : name(name), adr(adr), len(len), patternMatcher(patternMatcher) {}

bool Pattern::operator()(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end) {
    if ((end - begin) != len || *begin != adr) {
        return false;
    }

    return patternMatcher(begin, end);
}

bool CommandRecognizer::getCommand(CommandView &command) {

    if (recognized.empty()) {
        return false;
    } else {
        command = recognized.front();
        recognized.pop();
        return true;
    }
}

CommandRecognizer &CommandRecognizer::operator<<(Buffer &message) {
    auto begin = message.data.begin();
    auto end = begin + message.size;

    while (begin < end ||
           (!currentPatternName.empty() && (buffer.size() >= (currentPatternLength + commandStartPosition)))) {
        if (currentPatternName.empty()) {
            resetBuffer();
            if (!startNewPattern(begin, end)) {
                return *this;
            }
        }

        int filledBytes = buffer.size() - commandStartPosition;

        while (begin != end && filledBytes < currentPatternLength) {
            buffer.push_back(*begin);
            ++filledBytes;
            ++begin;
        }

        if (filledBytes >= currentPatternLength) {

            if (checkBuffer()) {
                auto first = buffer.begin() + commandStartPosition;
                auto last = first + currentPatternLength;
                recognized.emplace(currentPatternName, first, last);
                commandStartPosition += currentPatternLength;
                currentPatternName = "";

                startNewPatternFromBuffer();
            } else {
                gotoNextPattern();
            }

        } else {
            return *this;
        }
    }

    return *this;
}

void CommandRecognizer::addPattern(Pattern &&pattern) {
    uint8_t adr = pattern.getAdr();
    auto patterns = patternMatchers.find(adr);

    if (patterns == patternMatchers.end()) {
        patternMatchers[adr].push_back(pattern);
    } else {
        patterns->second.push_back(pattern);
        std::sort(patterns->second.begin(), patterns->second.end(),
                  [](const Pattern &a, const Pattern &b) { return a.getLen() < b.getLen(); });
    }

    if (!currentPatternName.empty()) {
        currentPattern = patternMatchers[currentPatternAdr].begin();
        while (currentPattern->getName() != currentPatternName) {
            ++currentPattern;
        }
    }
}

void CommandRecognizer::resetBuffer() {
    if (commandStartPosition == buffer.size() || buffer.size() > MAX_BUFFER_SIZE) {

        buffer.clear();
        commandStartPosition = 0;
    }
}

bool CommandRecognizer::startNewPattern(std::vector<uint8_t>::iterator &begin, std::vector<uint8_t>::iterator &end) {

    while (begin < end && patternMatchers.find(*begin) == patternMatchers.end()) {

        ++begin;
    }

    if (begin >= end) {
        currentPatternName = "";
        return false;
    }

    currentPattern = patternMatchers[*begin].begin();
    currentPatternName = currentPattern->getName();
    currentPatternAdr = currentPattern->getAdr();
    currentPatternLength = currentPattern->getLen();

    return true;
}

bool CommandRecognizer::checkBuffer() {
    if (currentPatternName.empty()) {

        return false;
    }

    auto start = buffer.begin() + commandStartPosition;

    return (*currentPattern)(start, start + currentPatternLength);
}

void CommandRecognizer::gotoNextPattern() {

    if (currentPatternName.empty()) {
        startNewPatternFromBuffer();
        return;
    }

    ++currentPattern;

    if (currentPattern != patternMatchers[currentPatternAdr].end()) {
        currentPatternName = currentPattern->getName();
        currentPatternAdr = currentPattern->getAdr();
        currentPatternLength = currentPattern->getLen();

        return;
    }

    ++commandStartPosition;
    startNewPatternFromBuffer();
}

void CommandRecognizer::startNewPatternFromBuffer() {
    if (commandStartPosition >= buffer.size()) {
        return;
    }

    auto commandStartIterator = buffer.begin() + commandStartPosition;
    auto endOfBuffer = buffer.end();

    auto beginNewCommand = commandStartIterator;

    startNewPattern(beginNewCommand, endOfBuffer);
    commandStartPosition = beginNewCommand - buffer.begin();
}
