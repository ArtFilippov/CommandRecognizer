#pragma once

#include "Pattern.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace patterns {
class MultiHeaderPatterns;
}

class patterns::MultiHeaderPatterns : public Pattern {

    std::unordered_map<uint8_t, std::shared_ptr<Pattern>> patterns;
    std::unordered_set<uint8_t> active;

  public:
    MultiHeaderPatterns(std::string name, std::vector<std::shared_ptr<Pattern>> patterns);

    std::string name() override;

    status proccess(uint8_t newByte) override;

    std::optional<std::size_t> len() override;

    void reset() override;

  private:
    void resetExceptOne(uint8_t hd);
};
