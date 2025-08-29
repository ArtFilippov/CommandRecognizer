#include "SingleHeaderPatterns.h"

using namespace patterns;

SingleHeaderPatterns::SingleHeaderPatterns(uint8_t header_, std::string name_,
                                           std::vector<std::shared_ptr<Pattern>> patterns_)
    : Pattern(header_, name_) {
    patterns.reserve(patterns_.size());

    for (auto p : patterns_) {
        if (p->header() == header()) {
            patterns.push_back(p);
        } else {
            throw std::invalid_argument{"Pattern with different header"};
        }
    }
}

Pattern::status SingleHeaderPatterns::proccess(uint8_t newByte) {
    if (step == automaton::WAIT_HEADER) {
        if (newByte == header()) {
            for (std::size_t i = 0; i < patterns.size(); ++i) {
                active.insert(i);
            }
            step = automaton::WAIT_MATCH;
        } else {
            return status::FAILED;
        }
    }

    for (auto it = active.begin(); it != active.end();) {
        auto res = patterns[*it]->proccess(newByte);
        if (res == status::FAILED) {
            patterns[*it]->reset();
            it = active.erase(it);
        } else if (res == status::MATCHED || res == status::MATCHED_STEP_BEFORE) {
            auto idx = *it;
            active.clear();
            active.insert(idx);
            return res;
        }
        ++it;
    }

    if (active.size() == 0) {
        return status::FAILED;
    }

    return status::NOT_COMPLETED;
}

std::optional<std::size_t> SingleHeaderPatterns::len() {
    if (active.size() == 1) {
        return patterns[*active.begin()]->len();
    }

    return {};
}

void SingleHeaderPatterns::reset() {
    step == automaton::WAIT_HEADER;

    for (auto idx : active) {
        patterns[idx]->reset();
    }

    active.clear();
}
