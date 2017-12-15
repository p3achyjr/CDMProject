#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "urm.cpp"

class Interpreter {

};

RM *parseRm(std::string content) {
  std::vector<std::string> instStrings;
  std::string token;
  std::istringstream tokenStream(content);
  while (std::getline(tokenStream, token, '\n')) {
    instStrings.push_back(token);
  }

  std::vector<Instr*> program;
  for (int i = 0; i < instStrings.size(); i++) {
    std::vector<std::string> tokens;
    std::istringstream stream(instStrings.at(i));
    while (std::getline(stream, token, ' ')) {
      tokens.push_back(token);
    }

    if (tokens.at(0).compare("inc") == 0) {
      program.push_back(
        (new Instr())->setInc(
          stoi(tokens.at(1)),
          stoi(tokens.at(2))));
    } else if (tokens.at(0).compare("dec") == 0) {
      program.push_back(
        (new Instr())->setDec(
          stoi(tokens.at(1)),
          stoi(tokens.at(2)),
          stoi(tokens.at(3))));
    } else {
      // halt
      program.push_back((new Instr())->setHalt());
    }
  }

  return new RM(program);
}

int main(int argc, char **argv) {
  printf("%s\n", argv[1]);
  std::ifstream ifs(argv[1]);
  std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

  RM *rm = parseRm(content);
  BitString *codeNum = rm->code();
  // std::cout << content << "\n";

  URM *urm = new URM(new BitString(1), codeNum);
  urm->step();
}