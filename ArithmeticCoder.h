//
// Created by andrija on 10/3/26.
//

#ifndef VAJA1_ARITHMETICCODER_H
#define VAJA1_ARITHMETICCODER_H

#include <array>
#include <cstdint>
#include <fstream>

class ArithmeticCoder {
private:
    static constexpr int BITS = 32;

    uint32_t lowerBound = 0;
    uint32_t upperBound = 0;
    uint32_t firstQuarter = 0;
    uint32_t secondQuarter = 0;
    uint32_t thirdQuarter = 0;

    struct Symbol {
        uint32_t frequency = 0;
        uint32_t low = 0;
        uint32_t high = 0;
    };

public:
    void compress(std::ifstream& input, std::ofstream& output);
    void decompress(std::ifstream& input, std::ofstream& output);
};


#endif //VAJA1_ARITHMETICCODER_H
