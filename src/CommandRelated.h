#pragma once

#include <vector>
#include <string>
#include "common/common.h"

struct CommandView {
    std::string name;
    std::vector<uint8_t> data;

    CommandView() {}
    CommandView(std::string name, std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end)
        : name(name), data(begin, end) {}
};
