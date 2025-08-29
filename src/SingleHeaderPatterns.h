#pragma once

#include "Pattern.h"
#include <vector>
#include <unordered_set>

namespace patterns {
class SingleHeaderPatterns;
}

class patterns::SingleHeaderPatterns : public Pattern {

    std::vector<std::shared_ptr<Pattern>> patterns;
    std::unordered_set<std::size_t> active;

    enum class automaton : uint8_t { WAIT_HEADER, WAIT_MATCH };
    automaton step;

  public:
    SingleHeaderPatterns(uint8_t header, std::string name, std::vector<std::shared_ptr<Pattern>> patterns);

    status proccess(uint8_t newByte) override;

    std::optional<std::size_t> len() override;

    void reset() override;
};
