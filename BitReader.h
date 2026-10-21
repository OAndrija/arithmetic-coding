#ifndef VAJA1_BITREADER_H
#define VAJA1_BITREADER_H
#include <fstream>

class BitReader {
    std::ifstream& input;
    unsigned char buffer = 0;
    int count = 0;

public:
    explicit BitReader(std::ifstream& input) : input(input) {}

    int readBit() {
        if (count == 0) {
            char c;
            if (!input.get(c)) return 0;    //padding zeros
            buffer = static_cast<unsigned char>(c);
            count = 8;
        }
        count--;
        return (buffer >> count) & 1;
    }
};

#endif //VAJA1_BITREADER_H
