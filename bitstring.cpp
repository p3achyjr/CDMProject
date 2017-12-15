#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

class BitString {
public:
  std::vector<bool> bits = std::vector<bool>(0);

  BitString(int initVal) {
    while (initVal > 0) {
      this->bits.push_back(initVal % 2);
      initVal /= 2;
    }
  };

  BitString(const BitString& copy)
    : bits(std::vector<bool>(copy.bits))
  { }

  bool isZero() {
    for (int i = 0; i < this->bits.size(); i++) {
      if (this->bits[i] == 1) return false;
    }

    return true;
  }

  bool isOdd() {
    return bits.at(0) == 1;
  }

  void inc() {
    int i = 0;
    for (; i < this->bits.size(); i++) {
      this->bits[i] = this->bits[i] ? 0 : 1;

      // if bits[i] was 0, then we've now reached the end of carry propagation
      if (this->bits[i]) break;
    }

    // in case bitstring was entirely ones
    if (i == this->bits.size())
      this->bits.push_back(1);
  }

  void dec() {
    int i = 0;
    for (; i < this->bits.size(); i++) {
      this->bits[i] = this->bits[i] ? 0 : 1;

      // likewise, but applies if bits[i] was 1
      if (!this->bits[i]) break;
    }

    if (i >= this->bits.size() - 1)
      this->bits.pop_back();
  }

  void mul2() {
    this->bits.insert(this->bits.begin(), 0);
  }

  void div2() {
    this->bits.erase(this->bits.begin());
  }

  void print() {
    if (this->isZero()) printf("0");
    for (int i = 0; i < this->bits.size(); i++)
      printf(this->bits[i] ? "1" : "0");

    printf("\n");
  }

  static BitString *pair(BitString *x, BitString *y) {
    // just need to tack on a 1 and x number of zeroes to y
    BitString *X = x;
    BitString *Y = y;
    
    Y->mul2();
    Y->inc();
    while (!X->isZero()) {
      Y->mul2();
      X->dec();
    }

    return Y;
  }
};

// int main() {
//   BitString s(15);
//   std::string str(s.bits.begin(), s.bits.end());
//   s.print();
//   s.inc();
//   s.print();
//   s.dec();
//   s.print();
// }
