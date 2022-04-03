#include "BitInputStream.hpp"

void BitInputStream::fill() {
    char c;
    int i;

    // read bytes into buffer until we reach eof
    for (i = 0; i < bufSize; i++) {
        if (!in.get(c)) break;
        buf[i] = c;
    }

    // potentially trigger eof flag
    in.peek();

    // set byteCap to amount of bytes we wrote to buffer
    byteCap = i;
}

bool BitInputStream::atEndOfFile() {
    if (nbits == ((byteCap * 8) - padding) && in.eof()) {
        return true;
    } else {
        return false;
    }
}

/* Returns true when trying to read past the end of the file, false otherwise */
bool BitInputStream::eof() { return eofBit; }

/* Returns true when trying to read past the end of the file, false otherwise */
void BitInputStream::setPadding(unsigned int n) { padding = n; }

unsigned int BitInputStream::readBit() {
    // check for EOF
    if (atEndOfFile()) {
        eofBit = 1;
        return 0;
    }

    // if we already read the entire buffer reset nbits
    if (nbits == (bufSize * 8)) nbits = 0;

    // fill or refill the buffer for reading
    if (nbits == 0) fill();

    // get Byte we are reading from
    int Idx = nbits / 8;
    char byte = buf[Idx];

    // construct bitmask
    char bitMask = 1;  // 00....01
    int bitShift = nbits % 8;
    bitShift = (8 - 1) - bitShift;
    bitMask = bitMask << bitShift;

    // extract bit
    unsigned char bit = byte & bitMask;

    // shift value back into last slot
    bit = bit >> bitShift;

    // update nbits
    nbits++;

    return bit;
}
