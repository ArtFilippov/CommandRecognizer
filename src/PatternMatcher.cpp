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
}

void PatternMatcher::proccess(uint8_t byte) {
    auto res = pattern->proccess(byte);

    if (res == Pattern::status::FAILED) {
        reset();
    } else if (res == Pattern::status::MATCHED) {
        buffer.push_back(byte);
        if (pattern->len() > buffer.size()) {
            throw std::logic_error{"Your pattern len is incorrect"};
        }
        recognized.push(Package{pattern->name(), buffer.end() - pattern->len(), buffer.end()});
        reset();
    } else if (res == Pattern::status::MATCHED_STEP_BEFORE) {
        if (buffer.empty()) {
            throw std::logic_error{"Your pattern MATCHED_STEP_BEFORE with no data"};
        }
        recognized.push(Package{pattern->name(), buffer.end() - pattern->len(), buffer.end()});
        reset();
        proccess(byte);
    } else {
        buffer.push_back(byte);
    }
}
