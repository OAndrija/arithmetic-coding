#include "ArithmeticCoder.h"

#include "BitWriter.h"

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

    //Number of bits
    output.put(static_cast<char>(BITS));

    //Symbol count so decoder knows when to stop reading symbols
    uint16_t symbolCount = 0;
    for (const auto& symbol : table) {
        if (symbol.frequency > 0) {
            symbolCount++;
        }
    }

    output.write(reinterpret_cast<const char*>(&symbolCount), sizeof(symbolCount));

    //Symbols
    for (int i = 0; i < 256; i++) {
        if (table[i].frequency > 0) {
            output.put(static_cast<char>(i));
        }
    }

    //Frequencies
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

        uint32_t step = (upperBound - lowerBound + 1) / cumulativeFrequency;
        upperBound = lowerBound + step * symbol.high - 1;
        lowerBound = lowerBound + step * symbol.low;

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

        while (lowerBound >= firstQuarter && upperBound < thirdQuarter) {
            lowerBound = 2 * (lowerBound - firstQuarter);
            upperBound = 2 * (upperBound - firstQuarter) + 1;
            E3Counter++;
        }
    }

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
    //TODO
}
