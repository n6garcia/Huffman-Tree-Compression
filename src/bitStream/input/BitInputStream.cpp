#include "BitInputStream.hpp"

/**
 * TODO: Fill the buffer of size bufSize from the input stream.
 */
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

/**
 * TODO: Return true when the istream tried to read more bytes than the
 * remaining bytes in the file and the buffer was also fully read. Otherwise,
 * return false.
 *
 * Note that eof() is different from this function. eof() in most other istreams
 * is only true when we are trying to read past the end of the file. Whereas
 * this function should be only true when we are at the end of the file, but
 * have not read past it yet. For example:
 *
 * // some code to read all the data in the file
 * atEndOfFile(); // atEndOfFile() = true. But eof() is still false for now.
 * readBit();     // trying to read past the end of the file
 * eof();         // eof is true now
 */
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

/**
 * TODO: Read the next bit from the bit buffer. Fills the buffer from the input
 * stream if all the bits have already been read. It should return 0 if the bit
 * read is 0, and return 1 if the bit read is 1.
 *
 * Before trying to read a bit from the buffer, if atEndOFfile() is true, set
 * the eofBit and directly return 0 (You can return any number here, since after
 * setting eofBit to true, eof() will return true and the program won’t be using
 * the returned value from readBit).
 */
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
