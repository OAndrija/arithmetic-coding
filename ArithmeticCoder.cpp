//
// Created by andrija on 10/3/26.
//

#include "ArithmeticCoder.h"

void ArithmeticCoder::compress(std::ifstream &input, std::ofstream &output) {
    //Symbol frequency table
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

    //Initialization
    lowerBound = 0;
    upperBound = (1ULL << (BITS - 1)) - 1;

    secondQuarter = (upperBound + 1) / 2;
    firstQuarter = secondQuarter / 2;
    thirdQuarter = firstQuarter * 3;

    uint32_t E3Counter = 0;

    while (input.get(byte)) {
        const Symbol& symbol = table[static_cast<unsigned char>(byte)];

        uint32_t step = (upperBound - lowerBound + 1) / cumulativeFrequency;
        upperBound = lowerBound + step * symbol.high - 1;
        lowerBound = lowerBound + step * symbol.low;

        while (upperBound < secondQuarter || lowerBound >= secondQuarter) {
            if (upperBound < secondQuarter) {
                //E1 -> Output 0, followed by E3Counter 1s

                lowerBound = lowerBound * 2;
                upperBound = upperBound * 2 + 1;
                //Izhod = 0 + E3_counter * 1
                E3Counter = 0;
            } else {
                //E2 -> Output 1, followed by E3Counter 0s
                lowerBound = 2 * (lowerBound - secondQuarter);
                upperBound = 2 * (upperBound - secondQuarter) + 1;
                //Izhod = 1 + E3_counter * 0;
                E3Counter = 0;
            }
        }

        while (lowerBound >= firstQuarter && upperBound < thirdQuarter) {
            lowerBound = 2 * (lowerBound - firstQuarter);
            upperBound = 2 * (upperBound - firstQuarter) + 1;
            E3Counter++;
        }
    }

    if (lowerBound < firstQuarter) {
        //IZhod: 01 + E3_counter * 1;
    } else {
        //Izgod: 10 + E3_counter * 0;
    }
}

void ArithmeticCoder::decompress(std::ifstream &input, std::ofstream &output) {
    //TODO
}
