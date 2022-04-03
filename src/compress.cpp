#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */
void pseudoCompression(const string& inFileName, const string& outFileName) {
    ifstream inFile;
    ofstream outFile;
    vector<unsigned int> freqs(256);
    char c;
    byte b;

    inFile.open(inFileName);
    outFile.open(outFileName);

    while (inFile.get(c)) {
        b = c;
        freqs[b]++;
    }

    HCTree tree;
    tree.build(freqs);

    inFile.clear();
    inFile.seekg(0, ios::beg);

    for (int i = 0; i < freqs.size(); i++) {
        outFile << freqs[i] << "\n";
    }

    while (inFile.get(c)) {
        b = c;
        tree.encode(b, outFile);
    }

    inFile.close();
    outFile.close();
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    ifstream inFile;
    ofstream outFile;

    vector<unsigned int> freqs(256);
    char c;
    byte b;
    int count = 0;

    inFile.open(inFileName);
    outFile.open(outFileName);

    BitOutputStream out(outFile, 4000);

    while (inFile.get(c)) {
        b = c;
        freqs[b]++;
        count++;
    }

    HCTree tree;
    tree.build(freqs);

    inFile.clear();
    inFile.seekg(0, ios::beg);

    // header **

    outFile << count << "\n";
    outFile << tree.serialize();

    // header end **

    while (inFile.get(c)) {
        b = c;
        tree.encode(b, out);
    }

    out.flush();

    inFile.close();
    outFile.close();
}

/* Main program that runs the compression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Compresses files using Huffman Encoding");
    options.positional_help("./path_to_input_file ./path_to_output_file");

    bool isAsciiOutput = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Write output in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAsciiOutput))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if original file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAsciiOutput) {
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}
