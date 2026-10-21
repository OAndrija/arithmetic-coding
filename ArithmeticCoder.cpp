#include "ArithmeticCoder.h"

#include <iostream>
#include <vector>

#include "BitWriter.h"
#include "BitReader.h"

void ArithmeticCoder::compress(std::ifstream &input, std::ofstream &output) {
    //Frequency table
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

    //Header

    //Number of bits (char -> 1 byte)
    output.put(static_cast<char>(BITS));

    //Symbol count so decoder knows when to stop reading symbols (uint16 -> 2 bytes)
    uint16_t symbolCount = 0;
    for (const auto& symbol : table) {
        if (symbol.frequency > 0) {
            symbolCount++;
        }
    }

    output.write(reinterpret_cast<const char*>(&symbolCount), sizeof(symbolCount));

    //Symbols (symbol count bytes , 1 symbol -> 1 byte)
    for (int i = 0; i < 256; i++) {
        if (table[i].frequency > 0) {
            output.put(static_cast<char>(i));
        }
    }

    //Frequencies (frequency writes 4 bytes for each symbol, 1 frequency -> 4 bytes)
    for (int i = 0; i < 256; i++) {
        if (table[i].frequency > 0) {
            output.write(reinterpret_cast<const char*>(&table[i].frequency), sizeof(uint32_t));
        }
    }

    input.clear();
    input.seekg(0);

    //Initialization
    lowerBound = 0;
    upperBound = (1ULL << (BITS - 1)) - 1;

    secondQuarter = (upperBound + 1) / 2;
    firstQuarter = secondQuarter / 2;
    thirdQuarter = firstQuarter * 3;

    uint32_t E3Counter = 0;
    BitWriter writer(output);

    while (input.get(byte)) {
        const Symbol& symbol = table[static_cast<unsigned char>(byte)];

        uint32_t step = (upperBound - lowerBound + 1) / cumulativeFrequency;    // Enacba 1
        upperBound = lowerBound + step * symbol.high - 1;   // Enacba 2
        lowerBound = lowerBound + step * symbol.low;    // Enacba 3

        // E1 and E2
        while (upperBound < secondQuarter || lowerBound >= secondQuarter) {
            if (upperBound < secondQuarter) {
                writer.writeBit(0);
                for (uint32_t i = 0; i < E3Counter; i++) {
                    writer.writeBit(1);
                }
                E3Counter = 0;

                lowerBound = lowerBound * 2;
                upperBound = upperBound * 2 + 1;
            } else {
                writer.writeBit(1);
                for (uint32_t i = 0; i < E3Counter; i++) {
                    writer.writeBit(0);
                }
                E3Counter = 0;

                lowerBound = 2 * (lowerBound - secondQuarter);
                upperBound = 2 * (upperBound - secondQuarter) + 1;
            }
        }

        // E3
        while (lowerBound >= firstQuarter && upperBound < thirdQuarter) {
            lowerBound = 2 * (lowerBound - firstQuarter);
            upperBound = 2 * (upperBound - firstQuarter) + 1;
            E3Counter++;
        }
    }

    // End rule
    if (lowerBound < firstQuarter) {
        writer.writeBit(0);
        writer.writeBit(1);
        for (uint32_t i = 0; i < E3Counter; i++) {
            writer.writeBit(1);
        }
        E3Counter = 0;
    } else {
        writer.writeBit(1);
        writer.writeBit(0);
        for (uint32_t i = 0; i < E3Counter; i++) {
            writer.writeBit(0);
        }
        E3Counter = 0;
    }
    writer.flush();
}

void ArithmeticCoder::decompress(std::ifstream &input, std::ofstream &output) {
    // Read header and rebuild the frequency table
    std::array<Symbol, 256> table{};
    char bitNumber;

    // Read number of bits (1 byte)
    if (!input.get(bitNumber)) {
        std::cerr << "Error reading bit number\n.";
        return;
    }
    const int bits = static_cast<unsigned char>(bitNumber);

    // Read symbol count (2 bytes)
    uint16_t symbolCount = 0;
    if (!input.read(reinterpret_cast<char*>(&symbolCount), sizeof(symbolCount))) {
        std::cerr << "Error reading symbol count\n";
        return;
    }

    // Read which byte values occur (symbolCount bytes read)
    std::vector<unsigned char> symbols(symbolCount);
    for (int i = 0; i < symbolCount; i++) {
        symbols[i] = static_cast<unsigned char>(input.get());
    }

    // Read symbol frequencies
    for (int i = 0; i < symbolCount; i++) {
        input.read(reinterpret_cast<char*>(&table[symbols[i]].frequency), sizeof(uint32_t));
    }

    // Calculate low, high and cumulative values, same as coder.
    uint32_t cumulativeFrequency = 0;
    for (auto &symbol : table) {
        symbol.low = cumulativeFrequency;
        cumulativeFrequency += symbol.frequency;
        symbol.high = cumulativeFrequency;
    }

    // Initialization
    lowerBound = 0;
    upperBound = (1ULL << (bits - 1)) - 1;

    secondQuarter = (upperBound + 1) / 2;
    firstQuarter = secondQuarter / 2;
    thirdQuarter = firstQuarter * 3;

    BitReader reader(input);
    uint32_t area = 0;
    for (int i = 0; i < bits - 1; i++) {
        area = (area << 1) | reader.readBit();
    }

    uint32_t decodedCount = 0;

    while (decodedCount < cumulativeFrequency) {
        uint32_t step = (upperBound - lowerBound + 1) / cumulativeFrequency;    // E1
        uint32_t value = (area - lowerBound) / step;    // Decoded value

        // Find the symbol whose interval contains value
        int s = -1;
        for (int i = 0; i < 256; i++) {
            if (table[i].frequency > 0 && value >= table[i].low && value < table[i].high) {
                s = i;
                break;
            }
        }
        // If no such interval exists in table, error decoding
        if (s == -1) {
            std::cerr << "Error decoding input\n";
            return;
        }
        output.put(static_cast<char>(s));

        upperBound = lowerBound + step * table[s].high - 1; //Enacba 2
        lowerBound = lowerBound + step * table[s].low;  //Enacba 3

        // E1 and E2
        while (upperBound < secondQuarter || lowerBound >= secondQuarter) {
            if (upperBound < secondQuarter) {
                lowerBound = lowerBound * 2;
                upperBound = upperBound * 2 + 1;
                area = 2 * area + reader.readBit();
            } else {
                lowerBound = 2 * (lowerBound - secondQuarter);
                upperBound = 2 * (upperBound - secondQuarter) + 1;
                area = 2 * (area - secondQuarter) + reader.readBit();
            }
        }

        while (lowerBound >= firstQuarter && upperBound < thirdQuarter) {
            lowerBound = 2 * (lowerBound - firstQuarter);
            upperBound = 2 * (upperBound - firstQuarter) + 1;
            area = 2 * (area - firstQuarter) + reader.readBit();
        }

        decodedCount++;
    }
}
