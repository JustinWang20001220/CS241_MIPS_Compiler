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
  std::vector<vector<Token>> tokenLines;
  string line;
  int address = 0;
  try {
    // first loop loop up the labels and store them
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      tokenLines.push_back(tokenLine);
      // check for empty line
      if (tokenLine.size() == 0) {
        continue;;
      }
      // after that at least tokenLine[0] is valid
      // examing whether the first is a LABLE
      while (tokenLine[0].getKind() == Token::LABEL) {
        string w = (string) tokenLine[0].getLexeme();
        w.pop_back();
        if (labels.count(w) == 0) {
          labels.insert(pair<string, int>(w, address));
        } 
        else {
          std::cerr << "ERROR: same label name" << std::endl;
          return 1;
        }
        tokenLine.erase(tokenLine.begin());
        if (tokenLine.size() == 0) {
          break;
        }
      }
      // only increment address when there are actual instruction here
      if (tokenLine.size() != 0) {
        address += 4;
      }
    }
  } 
  catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  address = 0;
  // second loop check for valid label and shit
  for (auto tokenLine: tokenLines) {
    // check for empty line
    if (tokenLine.size() == 0) {
      continue;;
    }
    // after that at least tokenLine[0] is valid
    while (tokenLine[0].getKind() == Token::LABEL) {
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
      // int after .word
      if (tokenLine[1].getKind() == Token::INT) {
        if (tokenLine[1].toNumber() > INT32_MAX || tokenLine[1].toNumber() < INT32_MIN) {
          std::cerr << "ERROR: inputted word out of range (INT)" << std::endl;
          return 1;
        } 
        else {
          print_instr(tokenLine[1].toNumber());
        }
      } 
      // hexint after .word
      else if (tokenLine[1].getKind() == Token::HEXINT) {
        if (tokenLine[1].toNumber() > 0xffffffff) {
          std::cerr << "ERROR: inputted word out of range (HEXINT)" << std::endl;
          return 1;
        } 
        else {
          print_instr(tokenLine[1].toNumber());
        }
      }
      // word after .word
      else if (tokenLine[1].getKind() == Token::ID) {
        // check if it is a valid label
        string label_name = tokenLine[1].getLexeme();
        if (labels.count(label_name) == 0) { // no lable defined
          std::cerr << "ERROR: no label defined" << std::endl;
          return 1;
        }
        else {
          print_instr(labels[label_name]);
        }
      }
      else {
        std::cerr << "ERROR: not correct token type after .word" << std::endl;
        return 1;
      }
    } 
    else {
      std::cerr << "ERROR: incorrect token, or length or word" << std::endl;
      return 1;
    }
    address += 4;
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
