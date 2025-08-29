#include "PatternMatcher.h"

using namespace patterns;

PatternMatcher::PatternMatcher(std::shared_ptr<Pattern> pattern) : pattern(pattern) {}

std::optional<Package> PatternMatcher::package() {
    if (recognized.empty()) {
        return {};
    }

    auto pkg = recognized.front();
    recognized.pop();

    return pkg;
}

void PatternMatcher::reset() {
    buffer.clear();
    pattern->reset();
}

IPatternMatcher &PatternMatcher::operator<<(const std::vector<uint8_t> &segment) {
    for (auto byte : segment) {
        proccess(byte);
    }

    return *this;
}

void PatternMatcher::proccess(uint8_t byte) {
    auto res = pattern->proccess(byte);

    if (res == Pattern::status::FAILED) {
        reset();
    } else if (res == Pattern::status::MATCHED) {
        buffer.push_back(byte);
        auto len = pattern->len();
        if (len.value_or(SIZE_MAX) > buffer.size()) {
            throw std::logic_error{"Your pattern len is incorrect"};
        }
        recognized.push(Package{pattern->name(), buffer.end() - len.value(), buffer.end()});
        reset();
    } else if (res == Pattern::status::MATCHED_STEP_BEFORE) {
        auto len = pattern->len();
        if (len.value_or(0) == 0 || buffer.empty()) {
            throw std::logic_error{"Your pattern MATCHED_STEP_BEFORE with no data"};
        }
        recognized.push(Package{pattern->name(), buffer.end() - len.value(), buffer.end()});
        reset();
        proccess(byte);
    } else {
        buffer.push_back(byte);
    }
}
