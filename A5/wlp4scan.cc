#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <utility>
#include <set>
#include <array>
#include <vector>
#include <iostream>

using namespace std;

class Token;

std::vector<Token> scan(const std::string &input);

class Token
{
public:
  enum Kind
  {
    ID,
    NUM,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    BECOMES,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PCT,
    COMMA,
    SEMI,
    LBRACK,
    RBRACK,
    AMP,
    WHITESPACE,
    COMMENT,
    ERR, // this is the kind when there is a obvious error
  };

private:
  Kind kind;
  std::string lexeme;

public:
  Token(Kind kind, std::string lexeme) : kind(kind), lexeme(std::move(lexeme)) {}
  Kind getKind() const
  {
    return kind;
  }
  const string &getLexeme() const
  {
    return lexeme;
  }
};

std::ostream &operator<<(std::ostream &out, const Token &tok);

class ScanningFailure
{
  std::string message;

public:
  ScanningFailure(std::string message) : message(std::move(message)) {}

  const std::string &what() const
  {
    return message;
  }
};

std::ostream &operator<<(std::ostream &out, const Token &tok)
{
  switch (tok.getKind())
  {
  case Token::ID:
    if (tok.getLexeme() == "if") {
      out << "IF";
      break;
    } else if (tok.getLexeme() == "int") {
      out << "INT";
      break;
    } else if (tok.getLexeme() == "return") {
      out << "RETURN";
      break;
    }
     else if (tok.getLexeme() == "else") {
      out << "ELSE";
      break;
    } else if (tok.getLexeme() == "while") {
      out << "WHILE";
      break;
    } else if (tok.getLexeme() == "println") {
      out << "PRINTLN";
      break;
    } else if (tok.getLexeme() == "wain") {
      out << "WAIN";
      break;
    } else if (tok.getLexeme() == "new") {
      out << "NEW";
      break;
    } else if (tok.getLexeme() == "delete") {
      out << "DELETE";
      break;
    } else if (tok.getLexeme() == "NULL") {
      out << "NULL";
      break;
    } else {
      out << "ID";
      break;
    }
  case Token::NUM:
    if (tok.getLexeme().size() > 1 && tok.getLexeme()[0] == '0')
    {
      throw ScanningFailure("ERROR: e.g. 01111");
    }
    else if (tok.getLexeme().size() > 10)
    {
      throw ScanningFailure("ERROR: INT out of range");
    }
    else if (tok.getLexeme().size() <= 10 && stoll(tok.getLexeme()) > 2147483647)
    {
      throw ScanningFailure("ERROR: INT out of range");
    }
    else
    {
      out << "NUM";
    }
    break;
  case Token::LPAREN:
    out << "LPAREN";
    break;
  case Token::RPAREN:
    out << "RPAREN";
    break;
  case Token::LBRACE:
    out << "LBRACE";
    break;
  case Token::RBRACE:
    out << "RBRACE";
    break;
  case Token::BECOMES:
    out << "BECOMES";
    break;
  case Token::EQ:
    out << "EQ";
    break;
  case Token::NE:
    out << "NE";
    break;
  case Token::LT:
    out << "LT";
    break;
  case Token::GT:
    out << "GT";
    break;
  case Token::LE:
    out << "LE";
    break;
  case Token::GE:
    out << "GE";
    break;
  case Token::PLUS:
    out << "PLUS";
    break;
  case Token::MINUS:
    out << "MINUS";
    break;
  case Token::STAR:
    out << "STAR";
    break;
  case Token::SLASH:
    out << "SLASH";
    break;
  case Token::PCT:
    out << "PCT";
    break;
  case Token::COMMA:
    out << "COMMA";
    break;
  case Token::SEMI:
    out << "SEMI";
    break;
  case Token::LBRACK:
    out << "LBRACK";
    break;
  case Token::RBRACK:
    out << "RBRACK";
    break;
  case Token::AMP:
    out << "AMP";
    break;
  case Token::COMMENT:
    out << "COMMENT";
    break;
  case Token::WHITESPACE:
    out << "WHITESPACE";
    break;
  }
  out << " " << tok.getLexeme();

  return out;
}

class AsmDFA
{
public:
  enum State
  {
    // States that are also kinds
    ID = 0,
    NUM,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    BECOMES,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PCT,
    COMMA,
    SEMI,
    LBRACK,
    RBRACK,
    AMP,
    WHITESPACE,
    ERROR,
    COMMENT,

    // States that are not also kinds
    FAIL,
    NEGATE,
    START,
    DOLLARS,

    // Hack to let this be used easily in arrays. This should always be the
    // final element in the enum, and should always point to the previous
    // element.

    LARGEST_STATE = DOLLARS
  };

private:
  std::set<State> acceptingStates;
  std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

  /*
   * Converts a state to a kind to allow construction of Tokens from States.
   * Throws an exception if conversion is not possible.
   */
  Token::Kind stateToKind(State s) const
  {
    switch (s)
    {
    case ID:
      return Token::ID;
    case NUM:
      return Token::NUM;
    case LPAREN:
      return Token::LPAREN;
    case RPAREN:
      return Token::RPAREN;
    case LBRACE:
      return Token::LBRACE;
    case RBRACE:
      return Token::RBRACE;
    case BECOMES:
      return Token::BECOMES;
    case EQ:
      return Token::EQ;
    case NE:
      return Token::NE;
    case LT:
      return Token::LT;
    case GT:
      return Token::GT;
    case LE:
      return Token::LE;
    case GE:
      return Token::GE;
    case PLUS:
      return Token::PLUS;
    case MINUS:
      return Token::MINUS;
    case STAR:
      return Token::STAR;
    case SLASH:
      return Token::SLASH;
    case PCT:
      return Token::PCT;
    case COMMA:
      return Token::COMMA;
    case SEMI:
      return Token::SEMI;
    case LBRACK:
      return Token::LBRACK;
    case RBRACK:
      return Token::RBRACK;
    case AMP:
      return Token::AMP;
    case COMMENT:
      return Token::COMMENT;
    case WHITESPACE:
      return Token::WHITESPACE;
    default:
      throw ScanningFailure("ERROR: Cannot convert state to kind.");
    }
  }

public:
  /* Tokenizes an input string according to the Simplified Maximal Munch
   * scanning algorithm.
   */
  std::vector<Token> simplifiedMaximalMunch(const std::string &input) const
  {
    std::vector<Token> result;

    State state = start();
    std::string munchedInput;

    // We can't use a range-based for loop effectively here
    // since the iterator doesn't always increment.
    for (std::string::const_iterator inputPosn = input.begin();
         inputPosn != input.end();)
    {

      State oldState = state;
      state = transition(state, *inputPosn);

      if (!failed(state))
      {
        munchedInput += *inputPosn;
        oldState = state;

        ++inputPosn;
      }

      if (inputPosn == input.end() || failed(state))
      {
        if (accept(oldState))
        {
          result.push_back(Token(stateToKind(oldState), munchedInput));

          munchedInput = "";
          state = start();
        }
        else
        {
          if (failed(state))
          {
            munchedInput += *inputPosn;
          }
          throw ScanningFailure("ERROR: Simplified maximal munch failed on input: " + munchedInput);
        }
      }
    }

    return result;
  }

  /* Initializes the accepting states for the DFA.
   */
  AsmDFA()
  {
    acceptingStates = {ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, 
      BECOMES, EQ, NE, LT, GT, LE, GE, PLUS, MINUS, STAR, SLASH, 
      PCT, COMMA, SEMI, LBRACK, RBRACK, AMP, COMMENT, WHITESPACE};
    // Non-accepting states are DOT, MINUS, ZEROX, DOLLARS, START

    // Initialize transitions for the DFA
    for (size_t i = 0; i < transitionFunction.size(); ++i)
    {
      for (size_t j = 0; j < transitionFunction[0].size(); ++j)
      {
        transitionFunction[i][j] = FAIL;
      }
    }

    registerTransition(START, isalpha, ID);
    registerTransition(ID, isalnum, ID);
    registerTransition(START, "0123456789", NUM);
    registerTransition(NUM, "0123456789", NUM);
    registerTransition(START, "(", LPAREN);
    registerTransition(START, ")", RPAREN);
    registerTransition(START, "[", LBRACK);
    registerTransition(START, "]", RBRACK);
    registerTransition(START, "{", LBRACE);
    registerTransition(START, "}", RBRACE);
    registerTransition(START, "-", MINUS);
    registerTransition(START, "+", PLUS);
    registerTransition(START, "*", STAR);
    registerTransition(START, "%", PCT);
    registerTransition(START, "=", BECOMES);
    registerTransition(BECOMES, "=", EQ);
    registerTransition(START, ";", SEMI);
    registerTransition(START, ",", COMMA);
    registerTransition(START, "<", LT);
    registerTransition(LT, "=", LE);
    registerTransition(START, ">", GT);
    registerTransition(GT, "=", GE);
    registerTransition(START, "&", AMP);
    registerTransition(START, "!", NEGATE);
    registerTransition(NEGATE, "=", NE);
    registerTransition(START, "/", SLASH);
    registerTransition(SLASH, "/", COMMENT);
    registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
    registerTransition(START, isspace, WHITESPACE);
    registerTransition(WHITESPACE, isspace, WHITESPACE);
  }

  // Register a transition on all chars in chars
  void registerTransition(State oldState, const std::string &chars, State newState)
  {
    for (char c : chars)
    {
      transitionFunction[oldState][c] = newState;
    }
  }

  // Register a transition on all chars matching test
  // For some reason the cctype functions all use ints, hence the function
  // argument type.
  void registerTransition(State oldState, int (*test)(int), State newState)
  {

    for (int c = 0; c < 128; ++c)
    {
      if (test(c))
      {
        transitionFunction[oldState][c] = newState;
      }
    }
  }

  /* Returns the state corresponding to following a transition
   * from the given starting state on the given character,
   * or a special fail state if the transition does not exist.
   */
  State transition(State state, char nextChar) const
  {
    return transitionFunction[state][nextChar];
  }

  /* Checks whether the state returned by transition
   * corresponds to failure to transition.
   */
  bool failed(State state) const { return state == FAIL; }

  /* Checks whether the state returned by transition
   * is an accepting state.
   */
  bool accept(State state) const
  {
    return acceptingStates.count(state) > 0;
  }

  /* Returns the starting state of the DFA
   */
  State start() const { return START; }
};

std::vector<Token> scan(const std::string &input)
{
  static AsmDFA theDFA;
  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);
  std::vector<Token> newTokens;
  for (auto &token : tokens)
  {
    if (token.getKind() != Token::WHITESPACE && token.getKind() != Token::Kind::COMMENT)
    {
      newTokens.push_back(token);
    }
  }
  return newTokens;
}

int main()
{
  std::string line;
  try
  {
    while (getline(std::cin, line))
    {
      std::vector<Token> tokenLine = scan(line);
      for (auto &token : tokenLine)
      {
        if (token.getKind() != Token::WHITESPACE) {
          cout << token << endl;
        }
      }
    }
  }
  catch (ScanningFailure &f)
  {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  return 0;
}
