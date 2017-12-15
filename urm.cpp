#include <cstdint>
#include <vector>
#include "bitstring.cpp"

enum InstrTag { DEC, INC, HALT };
enum MacroTag { COPY, ZERO, POP, READ, WRITE, MINC, MDEC, MHALT };

static int x = 0;
static int E = 1;
static int R = 2;
static int I = 3;
static int p = 4;
static int r = 5;
static int AUX1 = 6;
static int AUX2 = 7;
static int AUX3 = 8;
static int AUX4 = 9;
static int AUX5 = 10;

static void printRegisters(std::vector<BitString*> registers) {
  for (int i = 0; i < registers.size(); i++)
    registers.at(i)->print();
  printf("\n");
}

class Instr {
public:
  Instr *setInc(int r, int k) {
    this->tag = INC;
    this->r = r;
    this->k = k;

    return this;
  }

  Instr *setDec(int r, int k, int l) {
    this->tag = DEC;
    this->r = r;
    this->k = k;
    this->l = l;

    return this;
  }

  Instr *setHalt() {
    this->tag = HALT;

    return this;
  }

  int step(std::vector<BitString*> registers) {
    switch(this->tag) {
      case INC:
        registers.at(r)->inc();
        return k;
      case DEC:
        if (registers.at(r)->isZero())
          return l;

        registers.at(r)->dec();
        return k;
      case HALT:
        return -1;
    }
  }

  BitString *code() {
    switch(this->tag) {
      case INC:
        return BitString::pair(new BitString(this->r),
          BitString::pair(
            new BitString(this->k),
            new BitString(0)));
      case DEC:
        return BitString::pair(new BitString(this->l),
          BitString::pair(new BitString(this->r),
            BitString::pair(
              new BitString(this->k),
              new BitString(0))));
      case HALT:
        return new BitString(0);
    }
  }

  void print() {
    switch(this->tag) {
      case INC:
        printf("inc %d %d\n", r, k);
        return;
      case DEC:
        printf("dec %d %d %d\n", r, k, l);
        return;
      case HALT:
        printf("halt\n");
        return;
    }
  }

private:
  InstrTag tag;
  int r;
  int k;
  int l;
};

class Macro {
public:
  // hardcode k and l as -1, -2.
  Macro *setCopy(int r, int s, int k) {
    this->tag = COPY;
    this->k = k;
    this->insts = this->getCopyInstrs(r, s, -1);

    return this;
  }

  Macro *setZero(int r, int k, int l) {
    this->tag = ZERO;
    this->k = k;
    this->l = l;
    this->insts = this->getZeroInstrs(r, -1, -2);

    return this;
  }

  Macro *setPop(int r, int s, int k) {
    this->tag = POP;
    this->k = k;
    this->insts = this->getPopInstrs(r, s, -1);

    return this;
  }

  Macro *setRead(int r, int t, int s, int k) {
    this->tag = READ;
    this->k = k;
    this->insts = this->getReadInstrs(r, t, s, -1);

    return this;
  }

  Macro *setWrite(int r, int t, int s, int k) {
    this->tag = WRITE;
    this->k = k;
    this->insts = this->getWriteInstrs(r, t, s, -1);

    return this;
  }

  Macro *setInc(int r, int k) {
    this->tag = MINC;
    this->k = k;
    Instr *insts[] = { (new Instr())->setInc(r, -1) };
    this->insts = std::vector<Instr*>(insts, insts + 1);

    return this;
  }

  Macro *setDec(int r, int k, int l) {
    this->tag = MDEC;
    this->k = k;
    this->l = l;
    Instr *insts[] = { (new Instr())->setDec(r, -1, -2) };
    this->insts = std::vector<Instr*>(insts, insts + 1);

    return this;
  }

  Macro *setHalt() {
    this->tag = MHALT;
    Instr *insts[] = { (new Instr())->setHalt() };
    this->insts = std::vector<Instr*>(insts, insts + 1);

    return this;
  }

  int step(std::vector<BitString*> registers) {
    this->pc = this->insts.at(this->pc)->step(registers);

    return this->pc;
  }

  // returns macro index
  int exec(std::vector<BitString*> registers) {
    while (this->pc >= 0) {
      this->step(registers);

      printf("program counter: %d\n", pc);
      if (pc >= 0) this->insts.at(pc)->print();
      printf("\n");
      printRegisters(registers);
    }

    return this->pc == -1 ? k : l;
  }

  int fastexec(std::vector<BitString*> registers) {
    BitString *T = registers.at(t);
    BitString *S = registers.at(s);
    BitString *R = registers.at(r);
    switch(this->tag) {
      case COPY:
        registers.at(s) = registers.at(r);
        return k;
      case ZERO:
        return registers.at(r)->isZero() ? k : l;
      case POP:
        while (!registers.at(r)->isOdd()) {
          registers.at(s) = new BitString(0);
          registers.at(r)->div2();
          registers.at(s)->inc();
        }

        registers.at(r)->div2();

        return k;
      case READ:
        while (!T->isZero()) {
          if (R->isOdd()) T->dec();
          R->div2();
        }

        registers.at(s) = new BitString(0);
        while (!R->isOdd()) {
          registers.at(s)->inc();
          R->div2();
        }

        return k;
      case WRITE:
        R = new BitString(1);
        printf("fuck\n");
        T->dec();
        T->print();
        while (!T->isZero()) {
          if (registers.at(r)->isOdd()) {
            T->dec();
            registers.at(r)->div2();
            R->mul2();
            R->inc();
          } else {
            registers.at(r)->div2();
            R->mul2();
          }
        }

        printf("here\n");

        while (!registers.at(r)->isOdd()) registers.at(r)->div2();
        while (!S->isZero()) {
          registers.at(r)->mul2();
          S->dec();
        }

        while (true) {
          R->dec();
          if (R->isZero()) break;
          R->inc();

          if (R->isOdd()) {
            registers.at(r)->mul2();
            registers.at(r)->inc();
          } else {
            registers.at(r)->mul2();
          }

          R->div2();

          return k;
        }
    }
  }

  MacroTag tag;
  int r;
  int s;
  int t;
  int k; // -1
  int l; // -2
  int pc = 0;
  std::vector<Instr*> insts = std::vector<Instr*>(0);

  std::vector<Instr*> getCopyInstrs(int r, int s, int k) {
    Instr *insts[] = {
      (new Instr())->setDec(s, 0, 1),
      (new Instr())->setDec(r, 2, 4),
      (new Instr())->setInc(s, 3),
      (new Instr())->setInc(AUX1, 1),
      (new Instr())->setDec(AUX1, 5, k),
      (new Instr())->setInc(r, 4)
    };

    return std::vector<Instr*>(insts, insts + 6);
  }

  std::vector<Instr*> getZeroInstrs(int r, int k, int l) {
    Instr *insts[] = {
      (new Instr())->setDec(r, k, 1),
      (new Instr())->setInc(r, l)
    };

    return std::vector<Instr*>(insts, insts + 2);
  }

  std::vector<Instr*> getPopInstrs(int r, int s, int k) {
    Instr *insts[] = {
      // zero registers
      (new Instr())->setDec(AUX1, 0, 1), // 0
      (new Instr())->setDec(s, 1, 2), // 1

      // repeatedly divide by 2 
      (new Instr())->setDec(r, 3, 5), // 2, even
      (new Instr())->setDec(r, 4, 10), // 3, odd
      (new Instr())->setInc(AUX1, 2), // 4

      // check if r' is 0. If so, r was 0
      (new Instr())->setDec(AUX1, 6, k), // 5
      (new Instr())->setInc(AUX1, 7), // 6
      (new Instr())->setInc(s, 8), // 7

      // move r' to r and repeat loop
      (new Instr())->setDec(AUX1, 9, 2), // 8
      (new Instr())->setInc(r, 8), // 9

      // move r' to r and terminate loop
      (new Instr())->setDec(AUX1, 11, k), // 10
      (new Instr())->setInc(r, 10) // 11
    };

    return std::vector<Instr*>(insts, insts + 12);
  }

  std::vector<Instr*> getReadInstrs(int r, int t, int s, int k) {
    int r1 = AUX1, t1 = AUX2, r2 = AUX3;
    Instr *insts[] = {
      // copy r into aux register
      (new Instr())->setDec(r1, 0, 1), // 0
      (new Instr())->setDec(t1, 1, 2), // 1
      (new Instr())->setDec(r, 3, 5), // 2
      (new Instr())->setInc(r1, 4), // 3
      (new Instr())->setInc(t1, 2), // 4
      (new Instr())->setDec(t1, 6, 7), // 5
      (new Instr())->setInc(r, 5), // 6

      // copy t into aux register
      (new Instr())->setDec(t1, 7, 8), // 7
      (new Instr())->setDec(r2, 8, 9), // 8
      (new Instr())->setDec(t, 10, 12), // 9
      (new Instr())->setInc(t1, 11), // 10
      (new Instr())->setInc(r2, 9), // 11
      (new Instr())->setDec(r2, 13, 14), // 12
      (new Instr())->setInc(t, 12), // 13

      (new Instr())->setDec(t1, 18, 24), // 14

      // divide r' in half, decrementing t' each time the least significant
      // bit is 1.
      (new Instr())->setDec(r1, 16, 18), // 15
      (new Instr())->setDec(r1, 17, 14), // 16
      (new Instr())->setInc(r2, 15), // 17

      // decrement t' if least significant bit of r' is 1
      (new Instr())->setDec(r2, 19, 15), // 18
      (new Instr())->setInc(r1, 18), // 19

      // at this point, the element we want is available, so we need to get it
      (new Instr())->setDec(r1, 21, 23), // 20
      (new Instr())->setDec(r1, 22, k), // 21
      (new Instr())->setInc(r2, 20), // 22

      // increment s every time we divide r' in half
      (new Instr())->setInc(s, 24), // 23
      (new Instr())->setDec(r2, 25, 20), // 24
      (new Instr())->setInc(r1, 24) // 25
    };

    return std::vector<Instr*>(insts, insts + 26);
  }

  std::vector<Instr*> getWriteInstrs(int r, int t, int s, int k) {
    int rstk = AUX1, t1 = AUX2, s1 = AUX3, r1 = AUX4, rstk1 = AUX5;
    Instr *insts[] = {
      // copy t into aux register
      (new Instr())->setDec(t1, 0, 1), // 0
      (new Instr())->setDec(rstk, 1, 2), // 1
      (new Instr())->setDec(t, 3, 5), // 2
      (new Instr())->setInc(t1, 4), // 3
      (new Instr())->setInc(rstk, 2), // 4
      (new Instr())->setDec(rstk, 6, 7), // 5
      (new Instr())->setInc(t, 5), // 6

      // copy s into aux register
      (new Instr())->setDec(s1, 7, 8), // 7
      (new Instr())->setDec(rstk, 8, 9), // 8
      (new Instr())->setDec(s, 10, 12), // 9
      (new Instr())->setInc(s1, 11), // 10
      (new Instr())->setInc(rstk, 9), // 11
      (new Instr())->setDec(rstk, 13, 14), // 12
      (new Instr())->setInc(s, 12), // 13

      // set rstk to 1, check whether t is initially 0
      (new Instr())->setInc(rstk, 15), // 14
      (new Instr())->setDec(t1, 16, 72), // 15

      // divide r in half repeatedly
      (new Instr())->setDec(r, 17, 19), // 16
      (new Instr())->setDec(r, 18, 24), // 17
      (new Instr())->setInc(r1, 16), // 18

      // if even, multiply rstk by 2
      (new Instr())->setDec(r1, 20, 21), // 19
      (new Instr())->setInc(r, 19), // 20
      (new Instr())->setDec(rstk, 22, 31), // 21
      (new Instr())->setInc(rstk1, 23), // 22
      (new Instr())->setInc(rstk1, 21), // 23

      // if odd, first check if t is 0. If so, continue to next phase
      // ow, multiply rstk by 2 and add 1
      (new Instr())->setDec(r1, 25, 26), // 24
      (new Instr())->setInc(r, 24), // 25
      (new Instr())->setDec(t1, 27, 33), // 26
      (new Instr())->setDec(rstk, 28, 30), // 27
      (new Instr())->setInc(rstk1, 29), // 28
      (new Instr())->setInc(rstk1, 27), // 29

      // add 1 and then copy rstk1 back to rstk
      (new Instr())->setInc(rstk1, 31), // 30
      (new Instr())->setDec(rstk1, 32, 16), // 31
      (new Instr())->setInc(rstk, 31), // 32

      // t1 is now 0, so we need to divide r in half until we get the next 1
      (new Instr())->setDec(r, 34, 36), // 33
      (new Instr())->setDec(r, 35, 38), // 34
      (new Instr())->setInc(r1, 33), // 35

      // if even, copy back
      (new Instr())->setDec(r1, 37, 33), // 36
      (new Instr())->setInc(r, 36), // 37

      // if odd, reverse operation (2r + 1)
      (new Instr())->setDec(r1, 39, 41), // 38
      (new Instr())->setInc(r, 40), // 39
      (new Instr())->setInc(r, 38), // 40
      (new Instr())->setInc(r, 42), // 41

      // write contents of s1 into r, i.e. every time we decrement s1, we
      // multiply r by 2
      (new Instr())->setInc(s1, 43), // 42
      (new Instr())->setDec(s1, 44, 49), // 43
      (new Instr())->setDec(r, 45, 47), // 44
      (new Instr())->setInc(r1, 46), // 45
      (new Instr())->setInc(r1, 44), // 46
      (new Instr())->setDec(r1, 48, 43), // 47
      (new Instr())->setInc(r, 47), // 48
      (new Instr())->setInc(r, 50), // 49

      // now write contents of rstk back to r
      // do this by repeatedly dividing rstk by 2, each time multiplying r
      // if rstk is <= 1, we are done
      (new Instr())->setDec(rstk, 51, k), // 50
      (new Instr())->setDec(rstk, 52, k), // 51
      (new Instr())->setInc(rstk, 53), // 52
      (new Instr())->setInc(rstk, 54), // 53

      // divide rstk by 2
      (new Instr())->setDec(rstk, 55, 57), // 54
      (new Instr())->setDec(rstk, 56, 64), // 55
      (new Instr())->setInc(rstk1, 54), // 56

      // if rstk was even, multiply r by 2
      (new Instr())->setDec(rstk1, 58, 59), // 57
      (new Instr())->setInc(rstk, 57), // 58
      (new Instr())->setDec(r, 60, 62), // 59
      (new Instr())->setInc(r1, 61), // 60
      (new Instr())->setInc(r1, 59), // 61
      (new Instr())->setDec(r1, 63, 50), // 62
      (new Instr())->setInc(r, 62), // 63

      // if rstk was odd, multiply r by 2 and add 1
      (new Instr())->setDec(rstk1, 65, 66), // 64
      (new Instr())->setInc(rstk, 64), // 65
      (new Instr())->setDec(r, 67, 69), // 66
      (new Instr())->setInc(r1, 68), // 67
      (new Instr())->setInc(r1, 66), // 68
      (new Instr())->setDec(r1, 70, 71), // 69
      (new Instr())->setInc(r, 69), // 70
      (new Instr())->setInc(r, 50), // 71

      // case for if t is initially 0
      (new Instr())->setDec(s1, 73, k), // 72
      (new Instr())->setDec(r, 74, 76), // 73
      (new Instr())->setInc(r1, 75), // 74
      (new Instr())->setInc(r1, 73), // 75
      (new Instr())->setDec(r1, 77, 72), // 76
      (new Instr())->setInc(r, 76), // 77
    };

    return std::vector<Instr*>(insts, insts + 78);
  }
};

class RM {
public:
  std::vector<Instr*> program;
  BitString *codeNum;
  BitString *in;
  RM(std::vector<Instr*> insts) {
    this->program = insts;
  }

  BitString *code() {
    BitString *res = new BitString(0);
    for (int i = this->program.size() - 1; i >= 0; i--) {
      res = BitString::pair(this->program.at(i)->code(), res);
    }

    this->codeNum = res;
    return res;
  }
};

class URM {
public:
  URM(BitString *in, BitString *rm) {
    Macro *macros[] = {
      (new Macro())->setCopy(E, R, 1), // 0
      (new Macro())->setWrite(R, p, x, 2), // 1
      (new Macro())->setRead(E, p, I, 3), // 2
      (new Macro())->setPop(I, r, 4), // 3,
      (new Macro())->setZero(I, 13, 5), // 4
      (new Macro())->setPop(I, p, 6), // 5
      (new Macro())->setRead(R, r, x, 7), // 6
      (new Macro())->setZero(I, 8, 9), // 7
      (new Macro())->setInc(x, 12), // 8
      (new Macro())->setZero(x, 10, 11), // 9
      (new Macro())->setPop(I, p, 2), // 10
      (new Macro())->setDec(x, 12, 12), // 11
      (new Macro())->setWrite(R, r, x, 2), // 12
      (new Macro())->setHalt() // 13
    };

    this->program = std::vector<Macro*>(macros, macros + 14);
    this->registers = std::vector<BitString*>(11);
    this->registers.at(x) = in;
    this->registers.at(E) = rm;

    for (int i = 2; i < this->registers.size(); i++)
      this->registers.at(i) = new BitString(0);
  }

  int step() {
    Macro *cMacro = this->program.at(this->pc);;
    this->pc = cMacro->exec(this->registers);

    return this->pc;
  }

  int stepi() {
    return this->program.at(this->pc)->step(this->registers);
  }

  void run() {
    while (this->program.at(this->pc)->tag != MHALT) {
      this->step();
    }
  }

  std::vector<Macro*> program;
  std::vector<BitString*> registers;
  int pc = 0;
};

int main() {
  std::vector<BitString*> registers = std::vector<BitString*>(11);
  registers.at(0) = new BitString(21);
  registers.at(1) = new BitString(2);
  registers.at(2) = new BitString(5);
  for (int i = 3; i < registers.size(); i++)
    registers.at(i) = new BitString(0);

  for (int i = 0; i < registers.size(); i++)
    registers.at(i)->print();
  printf("\n");

  // Macro *copy = (new Macro())->setCopy(0, 1, 2);
  // copy->exec(registers);
  // Macro *pop = (new Macro())->setPop(1, 2, 2);
  // pop->exec(registers);
  Macro *write = (new Macro())->setWrite(0, 1, 2, 5);
  write->fastexec(registers);

  for (int i = 0; i < registers.size(); i++)
    registers.at(i)->print();
  printf("\n");
}
