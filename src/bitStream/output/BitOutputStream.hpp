#ifndef BITOUTPUTSTREAM_HPP
#define BITOUTPUTSTREAM_HPP

#include <iostream>

typedef unsigned char byte;

using namespace std;

class BitOutputStream {
  private:
    ostream& out;          // reference to the output stream to use
    char* buf;             // buffer of an char array
    unsigned int bufSize;  // size of buffer in byte
    unsigned int nbits;    // number of bits have been writen to buf
    unsigned int padding;

  public:
    explicit BitOutputStream(ostream& os, unsigned int bufSize) : out(os) {
        this->bufSize = bufSize;
        nbits = 0;
        padding = 0;
        buf = new char[bufSize];
    };

    ~BitOutputStream() { delete buf; }

    void flush();

    void writeBit(unsigned int i);

    unsigned int getPadding();
};

#endif
