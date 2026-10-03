//
// Created by andrija on 10/3/26.
//

#ifndef VAJA1_ARITHMETICCODER_H
#define VAJA1_ARITHMETICCODER_H
#include <iosfwd>

class ArithmeticCoder {
public:
    void compress(std::ifstream& input, std::ofstream& output);
    void decompress(std::ifstream& input, std::ofstream& output);
};


#endif //VAJA1_ARITHMETICCODER_H
