#pragma once

#include "IPatternMatcher.h"
#include "Pattern.h"

#include <queue>

namespace patterns {
class PatternMatcher;
}

class patterns::PatternMatcher : public IPatternMatcher {

    std::vector<uint8_t> buffer;

    std::queue<Package> recognized;

    std::shared_ptr<Pattern> pattern;

  public:
    PatternMatcher(std::shared_ptr<Pattern> pattern);

    std::optional<Package> package() override;

    void reset() override;

    IPatternMatcher &operator<<(const std::vector<uint8_t> &segment) override;

  public:
    void proccess(uint8_t byte);
};
