#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: vaja1 <c/d> <input> <output>" << std::endl;
        return 1;
    }

    std::string op = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    std::ofstream outputFile(outputPath, std::ios::binary);
    std::ifstream inputFile(inputPath, std::ios::binary);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files" << std::endl;
        return 2;
    }

    if (op == "c") {

    } else if (op == "d") {

    } else {
        std::cerr << "Operation must be:\nc - compression\nd-decompression" << std::endl;
        return 1;
    }

    return 0;
}
