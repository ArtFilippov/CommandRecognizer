#pragma once

#include <vector>
#include <string>

struct CommandView {
    std::string name;
    std::vector<uint8_t> data;

    CommandView() {}
    CommandView(std::string name, std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end)
        : name(name), data(begin, end) {}
};

struct Buffer {
    std::vector<uint8_t> data;
    int size{0};

    Buffer(int capacity) {
        for (int i = 0; i < capacity; ++i) {
            data.push_back(0);
        }
    }
};
