#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

void pseudoDecompression(const string& inFileName, const string& outFileName) {
    ifstream inFile;
    ofstream outFile;
    vector<unsigned int> freqs(256);
    byte b;
    unsigned int u;

    inFile.open(inFileName);
    outFile.open(outFileName);

    // build freqs array from header
    for (int i = 0; i < freqs.size(); i++) {
        inFile >> u;
        freqs[i] = u;
    }

    HCTree tree;
    tree.build(freqs);

    // to get rid of "\n" char at end of header
    inFile.get();

    // read and decode
    while (!inFile.eof()) {
        b = tree.decode(inFile);

        if (inFile.eof()) break;

        outFile << b;
    }

    inFile.close();
    outFile.close();
}

void trueDecompression(const string& inFileName, const string& outFileName) {
    ifstream inFile;
    ofstream outFile;

    vector<unsigned int> freqs(256);
    byte b;
    char c;
    unsigned int u;
    unsigned int count;
    unsigned int counter = 0;

    inFile.open(inFileName);
    outFile.open(outFileName);

    // header **
    string s;
    getline(inFile, s);
    count = stoi(s);

    int byte_count;
    getline(inFile, s);
    byte_count = stoi(s);

    int symbol_count;
    getline(inFile, s);
    symbol_count = stoi(s);

    string bit_pattern;
    string symbols;

    for (int i = 0; i < byte_count; i++) {
        inFile.get(c);
        bit_pattern = bit_pattern + c;
    }

    for (int i = 0; i < symbol_count; i++) {
        inFile.get(c);
        symbols = symbols + c;
    }

    HCTree tree;
    tree.deserialize(bit_pattern, symbols);

    // get rid of extra /n
    inFile.get();

    // header end **

    BitInputStream in(inFile, 4000);

    // read and decode
    while (!in.eof()) {
        b = tree.decode(in);

        if (in.eof()) break;

        outFile << b;

        counter++;

        if (counter == count) break;
    }

    inFile.close();
    outFile.close();
}

/* Main program that runs the decompression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Uncompresses files using Huffman Encoding");
    options.positional_help(
        "./path_to_compressed_input_file ./path_to_output_file");

    bool isAscii = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Read input in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAscii))("input", "",
                                       cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit.");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // if compressed file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        exit(0);
    }

    if (isAscii) {
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
