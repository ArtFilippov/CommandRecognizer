#pragma once

#include <optional>
#include <cstdint>
#include <memory>
#include <set>
#include <map>
#include <queue>

#include "CommandRelated.h"

class PatternMatcher {

  public:
    class Pattern {
      protected:
        uint8_t header;
        std::string name;

      public:
        Pattern(uint8_t header, const std::string name) : header(header), name(name) {}

        virtual ~Pattern() = default;

        virtual uint8_t getHeader() { return header; }

        virtual const std::string &getName() { return name; }

        virtual std::optional<bool> add(uint8_t) = 0;

        virtual void reset() = 0;
    };

    using pattern_ptr = std::unique_ptr<Pattern>;

  private:
    std::map<uint8_t, std::vector<pattern_ptr>> patterns;

    std::map<uint8_t, std::vector<pattern_ptr>>::iterator currentPatternSet;

    std::set<int> activePatterns;

    std::vector<uint8_t> buffer;

    int commandStartPosition{0};

    std::queue<CommandView> recognized;

  public:
    static const int MAX_BUFFER_SIZE = 256;

    void addPattern(pattern_ptr pattern) { patterns[pattern->getHeader()].push_back(std::move(pattern)); }

    bool getCommand(CommandView &command);

    void fillActivePatternsWithCurrentSet();

    std::optional<bool> checkActivePatterns(uint8_t);

    bool gotoNextHeader();

    bool findHeaderInSegment(std::vector<uint8_t>::iterator &begin, std::vector<uint8_t>::iterator &end);

    void resetAll();

    void resetBuffer();

    PatternMatcher &operator<<(Buffer &segment);
};
