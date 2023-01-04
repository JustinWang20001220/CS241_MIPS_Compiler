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
    // check for whether it is a valid ID instruction
    else if (tokenLine[0].getKind() == Token::ID) {

      // check for jr
      if (tokenLine[0].getLexeme() == "jr") {
        if (tokenLine.size() == 2 && tokenLine[1].getKind() == Token::REG) {
          int reg_num = tokenLine[1].toNumber();
          if (reg_num >= 0 && reg_num <= 31) {
            int instr = (0 << 26) | (reg_num << 21) | (0 << 16) | (0 << 11) | (0 << 6) | 8 ;
            print_instr(instr);
          }
          else {
            std::cerr << "ERROR: jr registor out of range" << std::endl;
            return 1;
          }
        } 
        else {
          std::cerr << "ERROR: wrong length for jr or wrong type after jr" << std::endl;
          return 1;
        }
      }
      
      // check for jalr
      else if (tokenLine[0].getLexeme() == "jalr") {
        if (tokenLine.size() == 2 && tokenLine[1].getKind() == Token::REG) {
          int reg_num = tokenLine[1].toNumber();
          if (reg_num >= 0 && reg_num <= 31) {
            int instr = (0 << 26) | (reg_num << 21) | (0 << 16) | (0 << 11) | (0 << 6) | 9 ;
            print_instr(instr);
          }
          else {
            std::cerr << "ERROR: jalr registor out of range" << std::endl;
            return 1;
          }
        }
        else {
          std::cerr << "ERROR: wrong length for jalr or wrong type after jalr" << std::endl;
          return 1;
        }
      }

      // check for add/sub/slt/sltu instruction
      else if (tokenLine[0].getLexeme() == "add" || tokenLine[0].getLexeme() == "sub" 
      || tokenLine[0].getLexeme() == "slt" || tokenLine[0].getLexeme() == "sltu") {
        if (tokenLine.size() == 6) {
          if (tokenLine[2].getKind() != Token::COMMA || tokenLine[4].getKind() != Token::COMMA) {
            std::cerr << "ERROR: incorrect comma position/position for add" << std::endl;
            return 1;
          }
          else if (tokenLine[1].getKind() != Token::REG || tokenLine[3].getKind() != Token::REG || tokenLine[5].getKind() != Token::REG) {
            std::cerr << "ERROR: incorrect reg position/position for add" << std::endl;
            return 1;
          }
          else {
            int reg1 = tokenLine[1].toNumber();
            int reg2 = tokenLine[3].toNumber();
            int reg3 = tokenLine[5].toNumber();
            if (reg1 >= 0 && reg1 <= 31 && reg2 >= 0 && reg2 <= 31 && reg3 >= 0 && reg3 <= 31) {
              if (tokenLine[0].getLexeme() == "add") {
                int instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | (0 << 6) | 32 ;
                print_instr(instr);
              }
              else if (tokenLine[0].getLexeme() == "sub") {
                int instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | (0 << 6) | 34 ;
                print_instr(instr);
              }
              else if (tokenLine[0].getLexeme() == "slt") {
                int instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | (0 << 6) | 42 ;
                print_instr(instr);  
              }
              else {
                int instr = (0 << 26) | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | (0 << 6) | 43 ;
                print_instr(instr);  
              }
            }
            else {
              std::cerr << "ERROR: reg value out of range for add" << std::endl;
              return 1;
            }
          }
        }
        else {
          std::cerr << "ERROR: incorrect length for add instruction" << std::endl;
          return 1;
        }
      }
      // check for beq, bne
      else if (tokenLine[0].getLexeme() == "beq" || tokenLine[0].getLexeme() == "bne") {
        if (tokenLine[2].getKind() != Token::COMMA || tokenLine[4].getKind() != Token::COMMA) {
          std::cerr << "ERROR: incorrect comma position/position" << std::endl;
          return 1;
        }
        else if (tokenLine[1].getKind() != Token::REG || tokenLine[3].getKind() != Token::REG) {
          std::cerr << "ERROR: incorrect reg position/position" << std::endl;
          return 1;
        }
        else if (tokenLine[5].getKind() == Token::HEXINT) {
          if (tokenLine[5].toNumber() > 0xffff) {
            std::cerr << "ERROR: hex number out of range" << std::endl;
            return 1;
          } 
          else {
            int reg1 = tokenLine[1].toNumber();
            int reg2 = tokenLine[3].toNumber();
            int num = tokenLine[5].toNumber();
            if (tokenLine[0].getLexeme() == "beq") {
              int instr = (4 << 26) | (reg1 << 21) | (reg2 << 16) | num ;
              print_instr(instr);
            }
            else {
              int instr = (5 << 26) | (reg1 << 21) | (reg2 << 16) | num ;
              print_instr(instr);
            }
          }
        }
        else if (tokenLine[5].getKind() == Token::INT) {
          if (tokenLine[5].toNumber() > INT16_MAX || tokenLine[5].toNumber() < INT16_MIN) {
            std::cerr << "ERROR: int number out of range" << std::endl;
            return 1;
          } 
          else {
            int reg1 = tokenLine[1].toNumber();
            int reg2 = tokenLine[3].toNumber();
            int num = tokenLine[5].toNumber();
            if (num < 0) {
              if (tokenLine[0].getLexeme() == "beq") {
                int instr = (4 << 26) | (reg1 << 21) | (reg2 << 16) | (num & 0xFFFF);
                print_instr(instr);
              }
              else {
                int instr = (5 << 26) | (reg1 << 21) | (reg2 << 16) | (num & 0xFFFF);
                print_instr(instr);
              }
            } 
            else {
              if (tokenLine[0].getLexeme() == "beq") {
                int instr = (4 << 26) | (reg1 << 21) | (reg2 << 16) | num ;
                print_instr(instr);
              }
              else {
                int instr = (5 << 26) | (reg1 << 21) | (reg2 << 16) | num ;
                print_instr(instr);
              }
            }
          }
        }
      }

      else {
        std::cerr << "ERROR: unknown instruction" << std::endl;
        return 1;
      }
    }

    else {
      std::cerr << "ERROR: unkrown instruction" << std::endl;
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
