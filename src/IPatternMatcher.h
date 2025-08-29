#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "common/common.h"

namespace patterns {
struct Package {
    std::string name;
    std::vector<uint8_t> data;

    Package(std::string name, std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end)
        : name(name), data(begin, end) {}
};

class IPatternMatcher {
  public:
    virtual ~IPatternMatcher() = default;

    virtual std::optional<Package> package() = 0;

    virtual void reset() = 0;

    virtual IPatternMatcher &operator<<(const std::vector<uint8_t> &segment) = 0;
};
} // namespace patterns
