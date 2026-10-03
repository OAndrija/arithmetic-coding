//
// Created by andrija on 10/3/26.
//

#include "ArithmeticCoder.h"

void ArithmeticCoder::compress(std::ifstream &input, std::ofstream &output) {
    std::array<Symbol, 256> table{};
    uint32_t cumulativeFrequency = 0;

    char byte;
    while (input.get(byte)) {
        ++table[static_cast<unsigned char>(byte)].frequency;
    }

    for (auto &symbol : table) {
        symbol.low = cumulativeFrequency;
        cumulativeFrequency += symbol.frequency;
        symbol.high = cumulativeFrequency;
    }

    if (cumulativeFrequency == 0) return;

    input.clear();
    input.seekg(0);
}

void ArithmeticCoder::decompress(std::ifstream &input, std::ofstream &output) {
    //TODO
}
