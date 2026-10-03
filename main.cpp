#include <iostream>
#include <fstream>
#include <string>

#include "ArithmeticCoder.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: vaja1 <c/d> <input> <output>\n";
        return 1;
    }

    ArithmeticCoder arithmeticCoder;

    std::string op = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    std::ofstream outputFile(outputPath, std::ios::binary);
    std::ifstream inputFile(inputPath, std::ios::binary);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files\n";
        return 2;
    }

    if (op == "c") {
        arithmeticCoder.compress(inputFile, outputFile);
    } else if (op == "d") {
        arithmeticCoder.decompress(inputFile, outputFile);
    } else {
        std::cerr << "Operation must be:\n"
        << "c - compression\n"
        << "d - decompression\n";
        return 1;
    }

    return 0;
}
