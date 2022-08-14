# Huffman-Tree-Compression

## Intro
Implemented a Huffman Tree Data Structure and used it to perform lossless compression on file bytes.

## Huffman Tree
"Huffman coding is a lossless data compression algorithm. The idea is to assign variable-length codes to input characters, lengths of the assigned codes are based on the frequencies of corresponding characters. The most frequent character gets the smallest code and the least frequent character gets the largest code."
### https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
A huffman Tree 

## How it works
We take every byte from a file and encode it as a unsigned char (essentially an unsigned byte), then we use those chars that we find on a huffman tree to perform compression on the bytes within the file. We store a serialized version of the huffman tree within the compressed file for decompression later.
