#ifndef HCTREE_HPP
#define HCTREE_HPP

#include <fstream>
#include <queue>
#include <vector>

#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "HCNode.hpp"

using namespace std;

class HCTree {
  private:
    HCNode* root;            // the root of HCTree
    vector<HCNode*> leaves;  // a vector storing pointers to all leaf HCNodes
    int nodes;

  public:
    HCTree() { root = nullptr; }

    ~HCTree();

    void build(const vector<unsigned int>& freqs);

    void print();

    string serialize();

    void deserialize(string s1, string s2);

    void encode(byte symbol, BitOutputStream& out) const;

    void encode(byte symbol, ostream& out) const;

    byte decode(BitInputStream& in) const;

    byte decode(istream& in) const;
};

#endif  // HCTREE_HPP
