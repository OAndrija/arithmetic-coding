#include <chrono>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "ArithmeticCoder.h"

static double timeMs(bool compress, const std::string& in, const std::string& out) {
    ArithmeticCoder coder;
    std::ifstream inputFile(in, std::ios::binary);
    std::ofstream outputFile(out, std::ios::binary);

    auto start = std::chrono::steady_clock::now();
    if (compress) {
        coder.compress(inputFile, outputFile);
    } else {
        coder.decompress(inputFile, outputFile);
    }
    outputFile.close();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::milli>(end - start).count();
}

static void benchmark(const std::vector<std::string> &files, const std::string &csvPath) {
    const int RUNS = 100;
    std::ofstream csv(csvPath);
    csv << "file,original_bytes,compressed_bytes,compression_ratio,encode_ms,decode_ms\n";

    for (const auto &file : files) {
        double enc = 0, dec = 0;
        for (int i = 0; i < RUNS; i++) {
            enc += timeMs(true, file, "tmp.cmp");
            dec += timeMs(false, "tmp.cmp", "tmp.out");
        }

        double original = static_cast<double>(std::filesystem::file_size(file));
        double compressed = static_cast<double>(std::filesystem::file_size("tmp.cmp"));
        double ratio = compressed > 0 ? original / compressed : 0;

        csv << std::filesystem::path(file).filename().string() << ',' << original << ',' << compressed << ','
            << ratio << ',' << enc / RUNS << ',' << dec / RUNS << '\n';
        std::cout << file << ": ratio " << ratio << ", enc " << enc / RUNS
                  << " ms, dec " << dec / RUNS << " ms\n";
    }

    std::filesystem::remove("tmp.cmp");
    std::filesystem::remove("tmp.out");
}

int main(int argc, char *argv[]) {
    if (argc >= 4 && std::string(argv[1]) == "b") {
        benchmark(std::vector<std::string>(argv + 3, argv + argc), argv[2]);
        return 0;
    }

    if (argc != 4) {
        std::cerr << "Usage: vaja1 <c/d/> <input> <output>\n"
        << "    vaja1 <b> <results.csv> <file1> <file2> <...>\n";
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
