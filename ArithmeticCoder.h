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
