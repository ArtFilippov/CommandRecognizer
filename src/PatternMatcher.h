#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <map>
#include <queue>

#include "IPatternMatcher.h"

class PatternMatcher : public IPatternMatcher {

    std::map<uint8_t, std::vector<pattern_ptr>> patterns;

    std::map<uint8_t, std::vector<pattern_ptr>>::iterator currentPatternSet;

    std::set<int> activePatterns;

    std::vector<uint8_t> buffer;

    int commandStartPosition{0};

    std::queue<CommandView> recognized;

  public:
    void addPattern(pattern_ptr pattern) override { patterns[pattern->getHeader()].push_back(std::move(pattern)); }

    bool getCommand(CommandView &command) override;

    void reset() override;

    IPatternMatcher &operator<<(const std::vector<uint8_t> &segment) override;

  private:
    enum find_header_status { HEADER_FOUND, HEADER_NOT_FOUND };

    void resetBuffer();

    void fillActivePatternsWithCurrentSet();

    Pattern::status checkActivePatterns(uint8_t);

    find_header_status gotoNextHeader();

    find_header_status findHeaderInSegment(std::vector<uint8_t>::const_iterator &begin,
                                           std::vector<uint8_t>::const_iterator &end);
};
