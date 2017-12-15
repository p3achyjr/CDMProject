#include <vector>

class BitString {
public:
  std::vector<bool> bits;
  BitString(int);
  bool isZero();
  void inc();
  void dec();
  void print();
};
