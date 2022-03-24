#include "BitOutputStream.hpp"

/**
 * TODO: Write the part of the buffer that was written by the user to the output
 * stream, and then clear the buffer to allow further use. You may use fill() to
 * zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
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

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
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
