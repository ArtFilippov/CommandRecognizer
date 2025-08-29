#include "MultiHeaderPatterns.h"

using namespace patterns;

MultiHeaderPatterns::MultiHeaderPatterns(std::string name_, std::vector<std::shared_ptr<Pattern>> patterns_)
    : Pattern(0xff, name_) {
    patterns.reserve(patterns_.size());

    for (auto p : patterns_) {
        if (patterns.contains(p->header())) {
            throw std::invalid_argument{"Patterns with same headers"};
        } else {
            patterns[p->header()] = p;
        }
    }
}

Pattern::status MultiHeaderPatterns::proccess(uint8_t newByte) {
    if (patterns.contains(newByte)) {
        active.insert(newByte);
    }

    for (auto it = active.begin(); it != active.end();) {
        auto res = patterns[*it]->proccess(newByte);
        if (res == status::FAILED) {
            patterns[*it]->reset();
            it = active.erase(it);
        } else if (res == status::MATCHED || res == status::MATCHED_STEP_BEFORE) {
            resetExceptOne(*it);
            return res;
        }
        ++it;
    }

    if (active.size() == 0) {
        return status::FAILED;
    }

    return status::NOT_COMPLETED;
}

std::optional<std::size_t> MultiHeaderPatterns::len() {
    if (active.size() == 1) {
        return patterns[*active.begin()]->len();
    }

    return {};
}

void MultiHeaderPatterns::reset() {
    for (auto idx : active) {
        patterns[idx]->reset();
    }

    active.clear();
}

void MultiHeaderPatterns::resetExceptOne(uint8_t oneHd) {
    for (auto hd : active) {
        if (hd != oneHd) {
            patterns[hd]->reset();
            active.erase(hd);
        }
    }
}
