#include "HCTree.hpp"

#include <iostream>
#include <sstream>

void updateLeaves(HCNode* n, vector<HCNode*>& leaves);
void deleteNode(HCNode* node);
void printNode(HCNode* node);
void serializeTree(BitOutputStream& bos, stringstream& ssl, HCNode* node);
void deserializeTree(BitInputStream& bis, HCNode*& node, HCNode* parent);
void deserializeLeaves(stringstream& ssl, HCNode* node,
                       vector<HCNode*>& leaves);

HCTree::~HCTree() { deleteNode(root); }

string HCTree::serialize() {
    string s;

    stringstream ss;
    stringstream ssl;
    BitOutputStream bos(ss, 50);

    // loop through and serialize tree
    serializeTree(bos, ssl, root);
    bos.flush();

    // byte count
    s = s + to_string(ss.str().length()) + "\n";

    // symbol count
    s = s + to_string(leaves.size()) + "\n";

    // bit pattern
    s = s + ss.str();
    // symbols
    s = s + ssl.str() + "\n";

    return s;
}

/* preorder traversal */
void serializeTree(BitOutputStream& bos, stringstream& ssl, HCNode* node) {
    if (node == nullptr) {
        bos.writeBit(0);
        return;
    }

    // visit node
    bos.writeBit(1);

    if (!node->c0 && !node->c1) {
        ssl << node->symbol;
    }

    // first recur on left subtree
    serializeTree(bos, ssl, node->c0);

    // then recur on right subtree
    serializeTree(bos, ssl, node->c1);
}

void HCTree::deserialize(string s1, string s2) {
    stringstream ss;
    ss << s1;
    stringstream ssl;
    ssl << s2;
    BitInputStream bis(ss, 50);

    deserializeTree(bis, root, nullptr);

    deserializeLeaves(ssl, root, leaves);
}

void deserializeLeaves(stringstream& ssl, HCNode* node,
                       vector<HCNode*>& leaves) {
    if (node == nullptr) {
        return;
    }

    // visit node
    if (!node->c0 && !node->c1) {
        node->symbol = ssl.get();
        leaves.push_back(node);
    }

    // first recur on left subtree
    deserializeLeaves(ssl, node->c0, leaves);

    // then recur on right subtree
    deserializeLeaves(ssl, node->c1, leaves);
}

void deserializeTree(BitInputStream& bis, HCNode*& node, HCNode* parent) {
    int b = bis.readBit();

    if (b == 0) return;

    node = new HCNode(0, ' ', nullptr, nullptr, parent);
    deserializeTree(bis, node->c0, node);
    deserializeTree(bis, node->c1, node);
}

/**
 *  Helper function to delete a node and all its children
 *  uses a postorder traversal to recursively delete nodes
 *  credit to :
 * https://www.geeksforgeeks.org/tree-traversals-inorder-preorder-and-postorder/
 *  for the postOrder tree traversal algorithm
 *
 *  Note: leaves a dangling pointer on nodes parent.
 *  ( use with caution )
 *
 */
void deleteNode(HCNode* node) {
    if (node == nullptr) return;

    // first recur on left subtree
    deleteNode(node->c0);

    // then recur on right subtree
    deleteNode(node->c1);

    // now deal with the node
    delete node;
}

void HCTree::print() { printNode(root); }

void printNode(HCNode* node) {
    if (node == nullptr) return;

    // now deal with the node
    cout << (*node) << "\n";

    // first recur on left subtree
    printNode(node->c0);

    // then recur on right subtree
    printNode(node->c1);
}

void HCTree::build(const vector<unsigned int>& freqs) {
    // init PQ (forest)
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> PQ;

    // fill PQ
    for (auto it = freqs.begin(); it != freqs.end(); it++) {
        if (*it > 0) {
            PQ.push(
                new HCNode(*it, it - freqs.begin(), nullptr, nullptr, nullptr));
            nodes++;
        }
    }

    HCNode* t1;
    HCNode* t2;
    HCNode* t3;

    // while loop for building tree
    while (PQ.size() > 1) {
        t1 = PQ.top();
        PQ.pop();
        t2 = PQ.top();
        PQ.pop();
        t3 = new HCNode(t1->count + t2->count, t1->symbol, t1, t2, nullptr);
        nodes++;
        t1->p = t3;
        t2->p = t3;
        PQ.push(t3);
    }

    // update HCNODE root
    root = PQ.top();

    // update leaves vector
    updateLeaves(root, leaves);
}

/*
 * Credit to :
 * https://www.geeksforgeeks.org/print-leaf-nodes-left-right-binary-tree/
 * for the original algorithm used
 */
void updateLeaves(HCNode* n, vector<HCNode*>& leaves) {
    // if node is null, return
    if (!n) return;

    // if node is leaf node, add to leaves vector
    if (!n->c0 && !n->c1) {
        leaves.push_back(n);
        return;
    }

    // if left child exists, check for leaf
    // recursively
    if (n->c0) updateLeaves(n->c0, leaves);

    // if right child exists, check for leaf
    // recursively
    if (n->c1) updateLeaves(n->c1, leaves);
}

void HCTree::encode(byte symbol, BitOutputStream& out) const {
    vector<char> rev_encoding;
    HCNode* n;
    HCNode* p;

    // search for HCNODE w/ symbol equal to argument
    for (auto it = leaves.begin(); it != leaves.end(); it++) {
        if ((*it)->symbol == symbol) {
            n = *it;
        }
    }

    // traverse up tree from n to find rev_encoding
    while (n) {
        // p = n's parent
        p = n->p;
        // if n == root, hence n has no parent
        if (!p) break;

        // determine encoding from relation of p to n
        if (p->c0 == n) {
            rev_encoding.push_back(0);
        } else if (p->c1 == n) {
            rev_encoding.push_back(1);
        }

        // set n to p
        n = p;
    }

    // Special Edge Case : Tree has only one symbol
    if (leaves.size() == 1) rev_encoding.push_back(0);

    // pop rev_encoding to ostream out
    while (rev_encoding.size() != 0) {
        out.writeBit(rev_encoding.back());
        rev_encoding.pop_back();
    }
}

void HCTree::encode(byte symbol, ostream& out) const {
    vector<char> rev_encoding;
    HCNode* n;
    HCNode* p;

    // search for HCNODE w/ symbol equal to argument
    for (auto it = leaves.begin(); it != leaves.end(); it++) {
        if ((*it)->symbol == symbol) {
            n = *it;
        }
    }

    // traverse up tree from n to find rev_encoding
    while (n) {
        // p = n's parent
        p = n->p;
        // if n == root, hence n has no parent
        if (!p) break;

        // determine encoding from relation of p to n
        if (p->c0 == n) {
            rev_encoding.push_back('0');
        } else if (p->c1 == n) {
            rev_encoding.push_back('1');
        }

        // set n to p
        n = p;
    }

    // Special Edge Case : Tree has only one symbol
    if (leaves.size() == 1) rev_encoding.push_back('0');

    // pop rev_encoding to ostream out
    while (rev_encoding.size() != 0) {
        out << rev_encoding.back();
        cout << rev_encoding.back();
        rev_encoding.pop_back();
    }
}

byte HCTree::decode(BitInputStream& in) const {
    HCNode* n = root;
    char c;

    // Special edge case: one symbol
    if (leaves.size() == 1) {
        c = in.readBit();
        if (c == 0 || c == 1) {
            return root->symbol;
        } else {
            return 0;
        }
    }

    while (!in.eof()) {
        c = in.readBit();
        if (c == 0) {
            n = n->c0;
        } else if (c == 1) {
            n = n->c1;
        }

        if (!n->c0 && !n->c1) break;
    }

    if (!(!n->c0 && !n->c1)) return 0;

    return n->symbol;
}

byte HCTree::decode(istream& in) const {
    HCNode* n = root;
    char c;

    // Special edge case: one symbol
    if (leaves.size() == 1) {
        in.get(c);
        if (c == '0' || c == '1') {
            return root->symbol;
        } else {
            return 0;
        }
    }

    while (in.get(c)) {
        if (c == '0') {
            n = n->c0;
        } else if (c == '1') {
            n = n->c1;
        }

        if (!n->c0 && !n->c1) break;
    }

    if (!(!n->c0 && !n->c1)) return 0;

    return n->symbol;
}
