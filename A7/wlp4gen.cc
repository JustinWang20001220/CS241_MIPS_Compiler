#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

bool test = false;
typedef map<string, string> SymbolTable; // variable name, variable type
typedef map<string, pair<vector<string>, SymbolTable>> ProcedureTable; // procedure name, parameterlisttype, its symbol table

class Tree {
    public:
    class Node {
        public:
        Tree* belonged_tree;
        string token;
        string rule;
        string lexema;

        vector<Node*> children;

        Node(bool is_terminal, Tree* t) {
            this->belonged_tree = t;
            if (is_terminal) {
                string scanned_rule;
                getline(cin, scanned_rule);
                this->rule = scanned_rule;
                istringstream rule_ss(scanned_rule);
                rule_ss >> this->token;
                rule_ss >> this->lexema;
            } else {
                string scanned_rule;
                getline(cin, scanned_rule);
                this->rule = scanned_rule;
                istringstream rule_ss(scanned_rule);
                rule_ss >> this->token;
                string new_node_string;
                while (rule_ss >> new_node_string) {
                    bool child_is_terminal = this->belonged_tree->isTerminal(new_node_string);
                    if (child_is_terminal) {
                        Node* child_node_ptr = new Node(true, t);
                        this->children.push_back(child_node_ptr);
                    } else {
                        Node* child_node_ptr = new Node(false, t);
                        this->children.push_back(child_node_ptr);
                    }
                }
            }
        }

        void preOrderTraversal() {
            cerr << this->rule << endl;
            for (Node* n: this->children) {
                n->preOrderTraversal();
            }
        }

        ~Node() {
            for (Node* child: this->children) {
                delete child;
            }
        }
    };

    Node* root;
    vector<string> Terminals = {
        "BOF", "BECOMES", "COMMA", "ELSE", 
        "EOF", "EQ", "GE", "GT", "ID", "IF", 
        "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", 
        "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", 
        "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", 
        "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL",
    };
    ProcedureTable procedure_table;

    bool error = false;

    Tree() {
        this->root = new Node(false, this);
    }

    ~Tree() {
        delete this->root;
    }

    bool isTerminal(string test_string) {
        if (find(Terminals.begin(), Terminals.end(), test_string) != Terminals.end()) {
            return true;
        }
        return false;
    }

    bool checkExistingProcedure(string s) { // true if already procedure declared, false otherwise
        ProcedureTable::iterator it;
        it = this->procedure_table.find(s);
        if (it != this->procedure_table.end()) {
            return true;
        } else {
            return false;
        }
    }

    bool checkExistingSymbol(string procedure_name, string parameter_name) {
        SymbolTable::iterator it;
        it = this->procedure_table[procedure_name].second.find(parameter_name);
        if (it != this->procedure_table[procedure_name].second.end()) {
            return true;
        } else {
            return false;
        }
    }

    string typeString(string ts) {
        if (ts == "INT STAR") {
            return "int*";
        } else if (ts == "INT") {
            return "int"; 
        } else {
            return "unknown";
        }
    }

    void printTable() {
        for (ProcedureTable::iterator it = this->procedure_table.begin(); it != this->procedure_table.end(); ++it) {
            // if (it->first == "wain") {
            //     cerr << it->first << ":";
            //     for (string type: it->second.first) {
            //         cerr << " " << typeString(type);
            //     }
            //     cerr << endl;
            //     for (SymbolTable:: iterator it2 = it->second.second.begin(); it2 != it->second.second.end(); ++it2) {
            //         cerr << it2->first << " ";
            //         cerr << typeString(it2->second);
            //         cerr << endl;
            //     }
            // } else {
            //     cerr << it->first << ":";
            //     for (string type: it->second.first) {
            //         cerr << " " << typeString(type);
            //     }
            //     cerr << endl;
            // }
            cerr << it->first << ":";
            for (string type: it->second.first) {
                cerr << " " << typeString(type);
            }
            cerr << endl;
            for (SymbolTable:: iterator it2 = it->second.second.begin(); it2 != it->second.second.end(); ++it2) {
                cerr << it2->first << " ";
                cerr << typeString(it2->second);
                cerr << endl;
            }
        }
    }
    
    void buildSymbolTable(Tree::Node* n, string procedure_name) {
        if (n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            string parameter1_rule = n->children[3]->children[0]->rule;
            string parameter2_rule = n->children[5]->children[0]->rule;
            if (parameter1_rule == "type INT STAR") {
                this->procedure_table[procedure_name].first.push_back("INT STAR");
            } else {
                this->procedure_table[procedure_name].first.push_back("INT");
            }
            if (parameter2_rule == "type INT STAR") {
                this->procedure_table[procedure_name].first.push_back("INT STAR");
            } else {
                this->procedure_table[procedure_name].first.push_back("INT");
            }
        } else if (n->rule == "paramlist dcl COMMA paramlist" || n->rule == "paramlist dcl") {
            // check parameter
            string parameter_name = n->children[0]->children[1]->lexema;
            if (checkExistingSymbol(procedure_name, parameter_name)) {
                cerr << "ERROR: already declared varaible" << endl;
            } else {
                if (n->children[0]->children[0]->rule == "type INT STAR") {
                    this->procedure_table[procedure_name].first.push_back("INT STAR");
                } else if (n->children[0]->children[0]->rule == "type INT") {
                    this->procedure_table[procedure_name].first.push_back("INT");
                }
            }
        } 
        // else if (n->rule == "dcls dcls dcl BECOMES NUM SEMI" || n->rule == "dcls → dcls dcl BECOMES NULL SEMI") {
        //     // check declaration
        //     string parameter_name = n->children[1]->children[1]->lexema;
        //     if (checkExistingSymbol(procedure_name, parameter_name)) {
        //         cerr << "ERROR: already declared varaible" << endl;
        //     } else {
        //         if (n->children[1]->children[0]->rule == "type INT STAR") { {
        //             string parameter_type = "INT STAR";
        //             pair<string, string> symbol_row = make_pair(parameter_name, parameter_type);
        //             this->procedure_table[procedure_name].insert(symbol_row);
        //         } else if (n->children[1]->children[0]->rule == "type INT") {
        //             string parameter_type = "INT";
        //             pair<string, string> symbol_row = make_pair(parameter_name, parameter_type);
        //             this->procedure_table[procedure_name].insert(symbol_row);
        //         }
        //     }
        // } 
        else if (n->rule == "dcl type ID") {
            string parameter_name = n->children[1]->lexema;
            if (checkExistingSymbol(procedure_name, parameter_name)) {
                cerr << "ERROR: already declared varaible" << endl;
            } else {
                if (n->children[0]->rule == "type INT STAR") {
                    string parameter_type = "INT STAR";
                    pair<string, string> symbol_row = make_pair(parameter_name, parameter_type);
                    this->procedure_table[procedure_name].second.insert(symbol_row);
                } else if (n->children[0]->rule == "type INT") {
                    string parameter_type = "INT";
                    pair<string, string> symbol_row = make_pair(parameter_name, parameter_type);
                    this->procedure_table[procedure_name].second.insert(symbol_row);
                }
            }
        } else if (n->rule == "lvalue ID" || n->rule == "factor ID") {
            // check assignment for undeclared variable
            string parameter_name = n->children[0]->lexema;
            if (!checkExistingSymbol(procedure_name, parameter_name)) {
                cerr << "ERROR: assign undeclared variable" << endl;
            }
        } else if (n->rule == "factor ID LPAREN arglist RPAREN" || n->rule == "factor ID LPAREN RPAREN") {
            string called_procedure_name = n->children[0]->lexema;
            if (!checkExistingProcedure(called_procedure_name)) {
                cerr << "ERROR: call dedeclared function call" << endl;
            }
        }
        for (Tree::Node* child: n->children) {
            buildSymbolTable(child, procedure_name);
        }
    }

    void buildProcedureTables(Tree::Node* n) {
        if (n->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" ||
        n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" ) {
            string procedure_name = n->children[1]->lexema;
            if (checkExistingProcedure(procedure_name)) {
                cerr << "ERROR procedure already declared" << endl;
            } else {
                this->procedure_table.insert(make_pair(procedure_name, make_pair(vector<string>(), SymbolTable())));
                this->buildSymbolTable(n, procedure_name);
            }
        } else {
            for (Tree::Node* n: n->children) {
                buildProcedureTables(n);
            }
        }     
    }

    string getType(string procedure_name, string symbol_name) {
        return this->procedure_table[procedure_name].second[symbol_name];
    }

    string checkArgType(Tree::Node* n, string procedure_name, string caller_name, int para_position) {
        if (test) {
            cerr << "checkArgType called" << endl;
        }
        if (n->rule == "arglist expr") {
            string inputted_type = checkTypeWithProcedureName(n->children[0], caller_name);
            string targeted_type = this->procedure_table[procedure_name].first[para_position];
            if (false) {
                cerr << inputted_type << endl;
                cerr << targeted_type << endl;
            }
            if (inputted_type == "BAD" || targeted_type == "BAD" || inputted_type != targeted_type) {
                cerr << "ERROR: incorrect " << para_position << " position parameter for " << procedure_name << endl;
                return "BAD";
            } else {
                return "WELL TYPED1";
            }
        } else {
            string inputted_type = checkTypeWithProcedureName(n->children[0], caller_name);
            if (test) {
                cerr << "caller: " << caller_name << endl;
            }
            string targeted_type = this->procedure_table[procedure_name].first[para_position];
            if (inputted_type == "BAD" || targeted_type == "BAD" || inputted_type != targeted_type) {
                cerr << "ERROR: incorrect " << para_position << " position parameter &&&& for " << procedure_name << endl;
                cerr << inputted_type << "   VS    " << targeted_type << endl;
                return "BAD";
            } else {
                return checkArgType(n->children[2], procedure_name, caller_name, para_position+1);
            }
        }
    }

    int getArgLength(Tree::Node* n) {
        if (n->rule == "arglist expr") {
            return 1;
        } else {
            return 1 + getArgLength(n->children[2]);
        }
    }

    string checkTypeWithProcedureName(Tree::Node* n, string procedure_name) {
        if (test) {
            cerr << "checkTypeWithProcedureName: " << n->rule << ", procedure: " << procedure_name << endl;
        }
        if (n->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            if (checkTypeWithProcedureName(n->children[9], n->children[1]->lexema) != "INT") {
                cerr << "ERROR: procedure return type is not INT" << endl;
                return "BAD";
            } else {
                string result2 = checkTypeWithProcedureName(n->children[6], n->children[1]->lexema);
                string result3 = checkTypeWithProcedureName(n->children[7], n->children[1]->lexema);
                if (result2 == "BAD" || result3 == "BAD") {
                    return "BAD";
                } else {
                    return "INT";
                }
            }
        } else if (n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            if (checkTypeWithProcedureName(n->children[11], "wain") != "INT") {
                cerr << "ERROR: wain return type is not INT" << endl;
                return "BAD";
            } else {
                string result1 = checkTypeWithProcedureName(n->children[3], "wain");
                string result2 = checkTypeWithProcedureName(n->children[5], "wain");
                string result3 = checkTypeWithProcedureName(n->children[8], "wain");
                string result4 = checkTypeWithProcedureName(n->children[9], "wain");
                if (result2 == "BAD" || result3 == "BAD" || result1 == "BAD" || result4 == "BAD") {
                    return "BAD";
                } else if (result2 != "INT") {
                    cerr << "ERROR: main parameter problem" << endl;
                    return "BAD";
                } else {
                    return "INT";
                }
            }
        } else if (n->rule == "factor ID" || n->rule == "lvalue ID") {
            if (test) {
                cerr << "variable: " << n->children[0]->lexema << " in " << procedure_name << " is type: " << getType(procedure_name, n->children[0]->lexema) << endl;
            }
            return getType(procedure_name, n->children[0]->lexema);
        } else if (n->rule == "type INT") {
            return "INT";
        } else if (n->rule == "type INT STAR") {
            return "INT STAR";
        } else if (n->rule == "factor NULL") {
            return "INT STAR";
        } else if (n->rule == "factor NUM") {
            return "INT";
        } else if (n->rule == "lvalue STAR factor" || n->rule == "factor STAR factor") {
            string recieved_type = checkTypeWithProcedureName(n->children[1], procedure_name);
            if (recieved_type != "INT STAR") {
                cerr << "ERROR: type error 1" << endl;
                return "BAD";
            } else {
                return "INT";
            }
        } else if (n->rule == "lvalue LPAREN lvalue RPAREN") {
            return checkTypeWithProcedureName(n->children[1], procedure_name);
        } else if (n->rule == "factor ID LPAREN RPAREN") {
            int procedure_parameter_length = this->procedure_table[n->children[0]->lexema].first.size();
            if (procedure_parameter_length != 0) {
                cerr << "ERROR: incorrect function call number of parameter";
                return "BAD";
            }
            return "INT";
        } else if (n->rule == "factor ID LPAREN arglist RPAREN") {
            if (test) {
                cerr << "Checking for function call with parameter" << "  function: " << n->children[0]->lexema << endl;
            }
            int arglistLength = getArgLength(n->children[2]);
            if (arglistLength == this->procedure_table[n->children[0]->lexema].first.size()) {
                string result = checkArgType(n->children[2], n->children[0]->lexema, procedure_name, 0);
                if (result == "BAD") {
                    cerr << "ERROR: incorrect number of argument or type" << endl;
                    // if (test) {
                    //     cerr << "function called problem ********" << endl;
                    //     cerr << n->rule << ":::::" << n->children[0]->lexema << endl;
                    // }
                    return "BAD";
                } else {
                    return "INT";
                }
            } else {
                cerr << "ERROR: incorrect number of argument" << endl;
                return "BAD";
            }
        } else if (n->rule == "factor NEW INT LBRACK expr RBRACK") {
            if (checkTypeWithProcedureName(n->children[3], procedure_name) != "INT") {
                cerr << "ERROR: type error 9484" << endl;
                return "BAD";
            }
            return "INT STAR";
        } else if (n->rule == "factor AMP lvalue") {
            if (checkTypeWithProcedureName(n->children[1], procedure_name) != "INT") {
                cerr << "ERROR: type error create int star";
                return "BAD";
            }
            return "INT STAR";
        } else if (n->rule == "factor LPAREN expr RPAREN") {
            return checkTypeWithProcedureName(n->children[1], procedure_name);
        } else if (n->rule == "term term PCT factor" || 
        n->rule == "term term SLASH factor" || 
        n->rule == "term term STAR factor") {
            string LHS = checkTypeWithProcedureName(n->children[0], procedure_name);
            string RHS = checkTypeWithProcedureName(n->children[2], procedure_name);
            if (LHS != "INT" || RHS != "INT") {
                cerr << "ERROR: percentage type";
                return "BAD";
            }
            return "INT";
        } else if (n->rule == "term factor" || n->rule == "expr term") {
            return checkTypeWithProcedureName(n->children[0], procedure_name);
        } else if (n->rule == "expr expr PLUS term") {
            string LHS = checkTypeWithProcedureName(n->children[0], procedure_name);
            string RHS = checkTypeWithProcedureName(n->children[2], procedure_name);
            // if (test) {
            //     cerr << "calling expression check plus" << endl;
            //     cerr << "LHS: " << LHS << endl;
            //     cerr << "RHS: " << RHS << endl;
            // }
            if (LHS == "INT" && RHS == "INT") {
                return "INT";
            } else if ((LHS == "INT" && RHS == "INT STAR") || (LHS == "INT STAR" && RHS == "INT")) {
                return "INT STAR";
            } else {
                cerr << "ERROR: expression type error" << endl;
                return "BAD";
            }
        } else if (n->rule == "expr expr MINUS term") {
            string LHS = checkTypeWithProcedureName(n->children[0], procedure_name);
            string RHS = checkTypeWithProcedureName(n->children[2], procedure_name);
            if ((LHS == "INT" && RHS == "INT") || (LHS == "INT STAR" && RHS == "INT STAR")) {
                return "INT";
            } else if (LHS == "INT STAR" && RHS == "INT") {
                return "INT STAR";
            } else {
                cerr << "ERROR: expression type error" << endl;
                return "BAD";
            }
        } else if (n->rule == "test expr EQ expr" || 
        n->rule == "test expr NE expr" ||
        n->rule == "test expr LT expr" ||
        n->rule == "test expr LE expr" ||
        n->rule == "test expr GE expr" ||
        n->rule == "test expr GT expr" || 
        n->rule == "statement lvalue BECOMES expr SEMI") {
            string LHS = checkTypeWithProcedureName(n->children[0], procedure_name);
            string RHS = checkTypeWithProcedureName(n->children[2], procedure_name);
            if (LHS == RHS) {
                return "WELL TYPED2";
            } else {
                cerr << "ERROR: Comparsion" << endl;
                return "BAD";
            }
        } else if (n->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
            if (checkTypeWithProcedureName(n->children[2], procedure_name) == "BAD") {
                cerr << "ERROR: if test not well typed" << endl;
                return "BAD";
            }
            string result1 = checkTypeWithProcedureName(n->children[5], procedure_name);
            string result2 = checkTypeWithProcedureName(n->children[9], procedure_name);
            if (result1 == "BAD" || result2 == "BAD") {
                return "BAD";
            } else {
                return "WELL TYPED4";
            }
        } else if (n->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
            if (checkTypeWithProcedureName(n->children[2], procedure_name) == "BAD") {
                cerr << "ERROR: while test not well typed" << endl;
                return "BAD";
            } else {
                return checkTypeWithProcedureName(n->children[5], procedure_name);
            }
        } else if (n->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
            if (checkTypeWithProcedureName(n->children[2], procedure_name) != "INT") {
                cerr << "ERROR: print expression not well typed" << endl;
                return "BAD";
            } else {
                return "WELL TYPED6";
            }
        } else if (n->rule == "statement DELETE LBRACK RBRACK expr SEMI") {
            if (checkTypeWithProcedureName(n->children[3], procedure_name) != "INT STAR") {
                cerr << "ERROR: delete not well typed" << endl;
                return "BAD";
            } else {
                return "WELL TYPED7";
            }
        } else if (n->rule == "dcl type ID") {
            return checkTypeWithProcedureName(n->children[0], procedure_name);
        } else if (n->rule == "dcls dcls dcl BECOMES NUM SEMI") {
            if (checkTypeWithProcedureName(n->children[1], procedure_name) != "INT") {
                cerr << "ERROR: declaration type error" << endl;
                return "BAD";
            } else {
                return checkTypeWithProcedureName(n->children[0], procedure_name);
            }
        } else if (n->rule == "dcls dcls dcl BECOMES NULL SEMI") {
            if (checkTypeWithProcedureName(n->children[1], procedure_name) != "INT STAR") {
                cerr << "ERROR: declaration type error" << endl;
                return "BAD";
            } else {
                return checkTypeWithProcedureName(n->children[0], procedure_name);
            }
        } else {
            for (Tree::Node* child: n->children) {
                if (checkTypeWithProcedureName(child, procedure_name) == "BAD") {
                    return "BAD";
                }
            }
            if (false) {
                cerr << "WELL TYPE8 RULE: " << n->rule << endl;
            }
            return "WELL TYPED8";
        }
    }

    void checkType(Tree::Node* n) {  // return false if there is problem
        if (n->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            string procedure_name = n->children[1]->lexema;
            checkTypeWithProcedureName(n, procedure_name);
        } else if (n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            checkTypeWithProcedureName(n, "wain");
        } else {
            for (Tree::Node* child: n->children) {
                checkType(child);
            }
        }
    }
};

int main() {
    Tree t = Tree();
    t.buildProcedureTables(t.root);
    t.printTable();
    t.checkTypeWithProcedureName(t.root, "do not care");
    return 1;
}