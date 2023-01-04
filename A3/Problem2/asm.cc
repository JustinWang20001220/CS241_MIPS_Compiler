#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <map>
#include "scanner.h"

using namespace std;
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
  std::map<std::string, int> labels;
  std::string line;
  try {
    
    int address = 0;
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      // cout << "line scanned" << endl;

      // check for empty line
      if (tokenLine.size() == 0) {
        continue;;
      }
      // after that at least tokenLine[0] is valid
      
      // examing whether the first is a LABLE
      while (tokenLine[0].getKind() == Token::LABEL) {
        // cout << "reading token" << endl;
        // cout << "tokenline size:" << tokenLine.size() << endl;
        string w = (string) tokenLine[0].getLexeme();
        w.pop_back();
        if (labels.count(w) == 0) {
          labels.insert(pair<string, int>(w, address));
          // cout << "token stored" << endl;
        } else {
          std::cerr << "ERROR: same label name" << std::endl;
          return 1;
        }
        tokenLine.erase(tokenLine.begin());
        if (tokenLine.size() == 0) {
          break;
        }
      }
      
      if (tokenLine.size() == 0) {
        continue;;
      }

      // check for whether it is a valid .word instruction
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
      address += 4;
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  for (auto label: labels) {
    std::cerr << label.first << " " << label.second << std::endl;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}
