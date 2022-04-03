#include "BitOutputStream.hpp"

void BitOutputStream::flush() {
    // calculate amount of bytes have been written to
    int bytesWritten;
    if (nbits == 0)
        return;
    else
        bytesWritten = 1 + ((nbits - 1) / 8);

    // write bytes written to out (ostream)
    for (int i = 0; i < bytesWritten; i++) {
        out << buf[i];
    }

    // clear buf
    char* begin = buf;
    char* end = begin + sizeof((*buf));
    fill(begin, end, 0);

    // calc padding
    padding = nbits % 8;
    if (padding != 0) padding = 8 - padding;

    // clear nbits
    nbits = 0;
}

/* helper function for decompression */
unsigned int BitOutputStream::getPadding() { return padding; }

void BitOutputStream::writeBit(unsigned int i) {
    // flush buffer if full
    if (nbits == (bufSize * 8)) flush();

    // get Byte we are writing to
    int Idx = nbits / 8;
    char byte = buf[Idx];

    // construct bitmask
    char bitMask = 1;  // 00....01
    int bitShift = nbits % 8;
    bitShift = (8 - 1) - bitShift;
    bitMask = bitMask << bitShift;

    // set bit
    if (i == 1) {
        byte = byte | bitMask;
    } else {
        bitMask = ~bitMask;
        byte = byte & bitMask;
    }

    // update byte and nbits
    buf[Idx] = byte;
    nbits++;
}
