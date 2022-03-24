#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCTreeFixture : public ::testing::Test {
  protected:
    HCTree tree;

  public:
    SimpleHCTreeFixture() {
        // initialization code here
        vector<unsigned int> freqs(256);
        freqs['p'] = 1;
        freqs[39] = 1;
        freqs['n'] = 2;
        freqs['m'] = 2;
        freqs['k'] = 2;
        freqs['h'] = 2;
        freqs['a'] = 3;
        freqs['u'] = 9;

        tree.build(freqs);
    }
};

TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('h', os);
    tree.encode('h', os);
    ASSERT_EQ(os.str(), "100100");
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("101");
    ASSERT_EQ(tree.decode(is), 'a');
}