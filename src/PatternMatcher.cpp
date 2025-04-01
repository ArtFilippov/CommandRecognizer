#include "PatternMatcher.h"

bool PatternMatcher::getCommand(CommandView &command) {
    if (recognized.empty()) {
        return false;
    } else {
        command = recognized.front();
        recognized.pop();
        return true;
    }
}

bool PatternMatcher::findHeaderInSegment(std::vector<uint8_t>::iterator &begin, std::vector<uint8_t>::iterator &end) {
    while (begin < end) {
        currentPatternSet = patterns.find(*begin);

        if (currentPatternSet != patterns.end()) {
            fillActivePatternsWithCurrentSet();
            return true;
        } else {
            ++begin;
        }
    }

    return false;
}

PatternMatcher &PatternMatcher::operator<<(Buffer &segment) {

    auto begin = segment.data.begin();
    auto end = begin + segment.size;

    while (begin < end) {
        if (activePatterns.empty() && findHeaderInSegment(begin, end) == false) {
            return *this;
        }

        buffer.push_back(*begin);

        auto verdict = checkActivePatterns(*begin);

        if (verdict.has_value() && verdict.value() == true) {
            resetAll();
        }

        if (verdict.has_value() && verdict.value() == false) {
            if (gotoNextHeader() == false) {
                resetBuffer();
            }
        }

        ++begin;
    }

    return *this;
}

bool PatternMatcher::gotoNextHeader() {

    bool isPatternFinished = true;

    while (commandStartPosition < buffer.size()) {
        auto new_begin = buffer.begin() + commandStartPosition;
        auto end = buffer.end();

        bool isFound = findHeaderInSegment(new_begin, end);

        commandStartPosition = std::distance(buffer.begin(), new_begin);

        if (isFound) {
            for (int i = commandStartPosition; i < (int) buffer.size(); ++i) {
                auto verdict = checkActivePatterns(buffer[i]);

                if (verdict.has_value() && verdict.value() == true) {
                    isPatternFinished = true;
                    commandStartPosition = i + 1;
                    break;
                }

                if (verdict.has_value() && verdict.value() == false) {
                    isPatternFinished = true;
                    ++commandStartPosition;
                    break;
                }

                isPatternFinished = false;
            }

            if (isPatternFinished == false) {
                return true;
            }
        }
    }

    return false;
}

std::optional<bool> PatternMatcher::checkActivePatterns(uint8_t new_byte) {
    for (auto it = activePatterns.begin(); it != activePatterns.end();) {
        auto verdict = currentPatternSet->second.at(*it)->add(new_byte);

        if (verdict.has_value()) {
            if (verdict.value()) {
                recognized.emplace(currentPatternSet->second.at(*it)->getName(), buffer.begin() + commandStartPosition,
                                   buffer.end());

                return {true};
            } else {
                currentPatternSet->second.at(*it)->reset();

                it = activePatterns.erase(it);
            }
        } else {
            ++it;
        }
    }

    if (activePatterns.empty()) {
        return {false};
    }

    return {};
}

void PatternMatcher::fillActivePatternsWithCurrentSet() {
    for (int i = 0; i < (int) currentPatternSet->second.size(); ++i) {
        activePatterns.insert(i);
    }
}

void PatternMatcher::resetBuffer() {
    buffer.clear();
    commandStartPosition = 0;
}

void PatternMatcher::resetAll() {
    resetBuffer();

    if (currentPatternSet != patterns.end()) {
        for (auto i : activePatterns) {
            currentPatternSet->second.at(i)->reset();
        }

        currentPatternSet = patterns.end();
    }

    activePatterns.clear();
}
