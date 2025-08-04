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

PatternMatcher::find_header_status PatternMatcher::findHeaderInSegment(std::vector<uint8_t>::const_iterator &begin,
                                                                       std::vector<uint8_t>::const_iterator &end) {
    while (begin < end) {
        currentPatternSet = patterns.find(*begin);

        if (currentPatternSet != patterns.end()) {
            fillActivePatternsWithCurrentSet();
            return HEADER_FOUND;
        } else {
            ++begin;
        }
    }

    return HEADER_NOT_FOUND;
}

IPatternMatcher &PatternMatcher::operator<<(const std::vector<uint8_t> &segment) {

    auto begin = segment.begin();
    auto end = segment.end();

    while (begin < end) {
        if (activePatterns.empty() && findHeaderInSegment(begin, end) == HEADER_NOT_FOUND) {
            return *this;
        }

        buffer.push_back(*begin);

        auto verdict = checkActivePatterns(*begin);

        if (verdict == Pattern::status::MATCHED) {
            reset();
        } else if (verdict == Pattern::status::FAILED) {
            if (gotoNextHeader() == HEADER_NOT_FOUND) {
                resetBuffer();
            }
        }

        ++begin;
    }

    return *this;
}

PatternMatcher::find_header_status PatternMatcher::gotoNextHeader() {

    bool isPatternFinished = true;

    while (commandStartPosition < buffer.size()) {
        auto new_begin = buffer.cbegin() + commandStartPosition;
        auto end = buffer.cend();

        auto isFound = findHeaderInSegment(new_begin, end);

        commandStartPosition = std::distance(buffer.cbegin(), new_begin);

        if (isFound == HEADER_FOUND) {
            for (int i = commandStartPosition; i < (int) buffer.size(); ++i) {
                auto verdict = checkActivePatterns(buffer[i]);

                if (verdict == Pattern::status::MATCHED) {
                    isPatternFinished = true;
                    commandStartPosition = i + 1;
                    break;
                }

                if (verdict == Pattern::status::FAILED) {
                    isPatternFinished = true;
                    ++commandStartPosition;
                    break;
                }

                isPatternFinished = false;
            }

            if (isPatternFinished == false) {
                return HEADER_FOUND;
            }
        } else {
            return HEADER_NOT_FOUND;
        }
    }

    return HEADER_NOT_FOUND;
}

PatternMatcher::Pattern::status PatternMatcher::checkActivePatterns(uint8_t new_byte) {
    for (auto it = activePatterns.begin(); it != activePatterns.end();) {
        auto &pattern = currentPatternSet->second.at(*it);
        auto verdict = pattern->add(new_byte);

        if (verdict == Pattern::status::NOT_COMPLETED) {
            ++it;
        } else if (verdict == Pattern::status::MATCHED) {
            recognized.emplace(pattern->getName(), buffer.begin() + commandStartPosition, buffer.end());
            return Pattern::status::MATCHED;
        } else {
            pattern->reset();
            it = activePatterns.erase(it);
        }
    }

    if (activePatterns.empty()) {
        return Pattern::status::FAILED;
    }

    return Pattern::status::NOT_COMPLETED;
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

void PatternMatcher::reset() {
    resetBuffer();

    if (currentPatternSet != patterns.end()) {
        for (auto i : activePatterns) {
            currentPatternSet->second.at(i)->reset();
        }

        currentPatternSet = patterns.end();
    }

    activePatterns.clear();
}
