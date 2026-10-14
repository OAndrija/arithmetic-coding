#ifndef VAJA1_BITWRITER_H
#define VAJA1_BITWRITER_H
#include <fstream>

class BitWriter {
    std::ofstream& output;
    unsigned char buffer = 0;
    int count = 0;

public:
    explicit BitWriter(std::ofstream& output) : output(output) {}

    void writeBit(int bit) {
        buffer = (buffer << 1) | bit;   //shift everything one place left, opens free spot on the right where we put bit
        count++;

        if (count == 8) {
            output.put(static_cast<char>(buffer));
            buffer = 0;
            count = 0;
        }
    }

    void flush() {
        while (count != 0) writeBit(0);   //padding empty bits with zeros

    }
};

#endif //VAJA1_BITWRITER_H
