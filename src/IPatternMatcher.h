#pragma once

#include "CommandRelated.h"

#include <cstdint>
#include <memory>

class IPatternMatcher {
  public:
    class Pattern {
      protected:
        uint8_t header;
        std::string name;

      public:
        enum status { MATCHED, FAILED, NOT_COMPLETED };

        Pattern(uint8_t header, const std::string name) : header(header), name(name) {}

        virtual ~Pattern() = default;

        virtual uint8_t getHeader() { return header; }

        virtual const std::string &getName() { return name; }

        virtual status add(uint8_t) = 0;

        virtual void reset() = 0;
    };

    using pattern_ptr = std::unique_ptr<Pattern>;

    virtual ~IPatternMatcher() = default;

    virtual void addPattern(pattern_ptr pattern) = 0;

    virtual bool getCommand(CommandView &command) = 0;

    virtual void reset() = 0;

    virtual IPatternMatcher &operator<<(common::Buffer &segment) = 0;
};
