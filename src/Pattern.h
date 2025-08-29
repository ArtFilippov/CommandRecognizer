#pragma once

#include <string>
#include <memory>
#include <optional>

namespace patterns {
class Pattern;

}

class patterns::Pattern {
    uint8_t header_;
    std::string name_;

  public:
    enum class status : uint8_t { MATCHED, FAILED, NOT_COMPLETED, MATCHED_STEP_BEFORE };

    Pattern(uint8_t header_, const std::string name_) : header_(header_), name_(name_) {}

    virtual ~Pattern() = default;

    virtual uint8_t header() { return header_; }

    virtual std::string name() { return name_; }

    virtual status proccess(uint8_t newByte) = 0;

    virtual std::optional<std::size_t> len() = 0;

    virtual void reset() = 0;
};
