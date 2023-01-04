#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */

static void print_instr(int instr) {
  unsigned char c = instr >> 24;
  std::cout << c;
  c = instr >> 16;
  std::cout << c;
  c = instr >> 8;
  std::cout << c;
  c = instr;
  std::cout << c;
}

int main() {
  std::string line;
  try {
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);

      // check for empty line
      if (tokenLine.size() == 0) {
        continue;;
      }

      if (tokenLine[0].getKind() == Token::WORD 
      && tokenLine[0].getLexeme() == ".word"
      && tokenLine.size() == 2) {
        if (tokenLine[1].getKind() == Token::INT) {
          if (tokenLine[1].toNumber() > INT32_MAX || tokenLine[1].toNumber() < INT32_MIN) {
            std::cerr << "ERROR: inputted word out of range (INT)" << std::endl;
            return 1;
          } else {
            print_instr(tokenLine[1].toNumber());
          }
        } else if (tokenLine[1].getKind() == Token::HEXINT) {
          if (tokenLine[1].toNumber() > 0xffffffff) {
            std::cerr << "ERROR: inputted word out of range (HEXINT)" << std::endl;
            return 1;
          } else {
            print_instr(tokenLine[1].toNumber());
          }
        } else {
          std::cerr << "ERROR: not correct token type after .word" << std::endl;
          return 1;
        }
      } else {
        std::cerr << "ERROR: incorrect token, or length or word" << std::endl;
        return 1;
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}
