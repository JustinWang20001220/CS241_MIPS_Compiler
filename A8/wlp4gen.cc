#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

bool test = false;
typedef map<string, pair<string, int>> SymbolTable; // variable name, <variable type, offset>
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
    map<string, int> offsets; // procedure name, current offset
    ProcedureTable procedure_table;
    bool import_print = false;
    int loop_count = 0;
    int endwhile_count = 0;
    int else_count = 0;
    int endif_count = 0;
    bool error = false;

    Tree() {
        this->root = new Node(false, this);
    }

    ~Tree() {
        delete this->root;
    }
    
    int getOffset(string procedure_name, string variable_name) {
        return this->procedure_table[procedure_name].second[variable_name].second;
    }

    void generateStatements(Tree::Node* n, string procedure_name) {}
    void generateReturnExpr(Tree::Node* n, string procedure_name) {}

    void push(int k) {
        cout << "sw $" << k << ", -4($30)" << endl;
        cout << "sub $30, $30, $4" << endl;
    }

    void pop(int k) {
        cout << "add $30,$30,$4" << endl;
        cout << "lw $" << k << ", -4($30)" << endl;
    }

    void generateCodeWithName(Tree::Node* n, string procedure_name) {
        if (n->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" 
        || n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" ) {
            if (procedure_name == "wain") {
                cout << "; begin prologue" << endl;
                cout << "lis $4" << endl;
                cout << ".word 4" << endl;
                cout << "lis $11" << endl;
                cout << ".word 1" << endl;
                cout << "sub $29 , $30, $4 ; update frame pointer" << endl;
                cout << "sw $1, 0($29) ; push variable a" << endl;
                cout << "sub $30 , $30, $4 ; update stack pointer" << endl;
                cout << "sw $2, -4($29) ; push variable b" << endl;
                cout << "sub $30 , $30, $4 ; update stack pointer" << endl;
                generateCodeWithName(n->children[8], procedure_name); // finding rest of the prologue from declaration
                cout << "; end prologue" << endl;
                generateCodeWithName(n->children[9], procedure_name); // finding rest of the prologue from statements
                generateCodeWithName(n->children[11], procedure_name); // finding rest of the prologue from return
                cout << "; begin epilogue" << endl;
                for (int i = 0; i < this->procedure_table[procedure_name].second.size(); ++i) {
                    cout << "add $30, $30, $4" << endl;
                }
                cout << "jr $31" << endl;
            }
        } 
        else if (n->rule == "factor LPAREN expr RPAREN") {
            generateCodeWithName(n->children[1], procedure_name);
        }
        else if (n->rule == "expr expr PLUS term") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "add $3, $5, $3" << endl;
        }
        else if (n->rule == "term term STAR factor") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "mult $5, $3" << endl;
            cout << "mflo $3" << endl;
        }
        else if (n->rule == "term term SLASH factor") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "div $5, $3" << endl;
            cout << "mflo $3" << endl;
        }
        else if (n->rule == "term term PCT factor") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "div $5, $3" << endl;
            cout << "mfhi $3" << endl;
        }
        else if (n->rule == "factor NUM") {
            cout << "lis $20" << endl;
            cout << ".word " << n->children[0]->lexema << endl;
            cout << "add $3, $20, $0" << endl;
        }
        else if (n->rule == "expr expr MINUS term") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        }
        else if (n->rule == "factor ID") {
            cout << "lw $3, " << getOffset(procedure_name, n->children[0]->lexema) << "($29); from the symbol table" << endl;
        }
        else if (n->rule == "statements statements statement") {
            generateCodeWithName(n->children[0], procedure_name);
            generateCodeWithName(n->children[1], procedure_name);
        }
        else if (n->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
            if (import_print == false) {
                cout << ".import print" << endl;
                import_print = true;
            }
            push(1);
            generateCodeWithName(n->children[2], procedure_name);
            cout << "add $1, $3, $0" << endl;
            push(31);
            cout << "lis $5" << endl;
            cout << ".word print" << endl;
            cout << "jalr $5" << endl;
            pop(31);
            pop(1);
        }
        else if (n->rule == "lvalue LPAREN lvalue RPAREN") {
            generateCodeWithName(n->children[1], procedure_name);
        }
        else if (n->rule == "dcls dcls dcl BECOMES NUM SEMI") {
            generateCodeWithName(n->children[0], procedure_name);
            generateCodeWithName(n->children[1], procedure_name);
            string number_assigned = n->children[3]->lexema;
            string variable_name = n->children[1]->children[1]->lexema;
            cout << "lis $3" << endl;
            cout << ".word " << number_assigned << endl;
            cout << "sw $3, " << getOffset(procedure_name, variable_name) << "($29)" << endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else if (n->rule == "statement lvalue BECOMES expr SEMI") {
            generateCodeWithName(n->children[2], procedure_name);
            string variable_name = n->children[0]->children[0]->lexema;
            cout << "sw $3, " << getOffset(procedure_name, variable_name) << "($29)" << endl;
            cout << "sub $30, $30, $4" << endl;
        }
        else if (n->rule == "test expr LT expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $3, $5, $3" << endl;
        }
        else if (n->rule == "test expr EQ expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
            cout << "add $3, $6, $7" << endl;
            cout << "sub $3, $11, $3" << endl;
        }
        else if (n->rule == "test expr NE expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
            cout << "add $3, $6, $7" << endl;
        }
        else if (n->rule == "test expr LE expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $3, $3, $5" << endl;
            cout << "sub $3, $11, $3" << endl;
        }
        else if (n->rule == "test expr GE expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $3, $5, $3" << endl;
            cout << "sub $3, $11, $3" << endl;
        }
        else if (n->rule == "test expr GT expr") {
            generateCodeWithName(n->children[0], procedure_name);
            push(3);
            generateCodeWithName(n->children[2], procedure_name);
            pop(5);
            cout << "slt $3, $3, $5" << endl;
        }
        else if (n->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
            int cur_loop_count = this->loop_count;
            int cur_endwhile_count = this->endwhile_count;
            this->loop_count += 1;
            this->endwhile_count += 1;
            cout << "loop" << cur_loop_count << ":" << endl;
            generateCodeWithName(n->children[2], procedure_name);
            cout << "beq $3 , $0 , endWhile" << cur_endwhile_count << endl;
            generateCodeWithName(n->children[5], procedure_name);
            cout << "beq $0, $0, loop" << cur_loop_count << endl;
            cout << "endWhile" << cur_endwhile_count << ":" << endl;
        }
        else if (n->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
            int cur_else_count = this->else_count;
            int cur_endif_count = this->endif_count;
            this->else_count += 1;
            this->endif_count += 1;
            generateCodeWithName(n->children[2], procedure_name);
            cout << "beq $3, $0, else" << cur_else_count << endl;
            generateCodeWithName(n->children[5], procedure_name);
            cout << "beq $0, $0, endif" << cur_endif_count << endl;
            cout << "else" << cur_else_count << ":" << endl;
            generateCodeWithName(n->children[9], procedure_name);
            cout << "endif" << cur_endif_count << ":" << endl;
        }
        else {
            for (Tree::Node* child: n->children) {
                generateCodeWithName(child, procedure_name);
            }
        }
    } 

    void generateCode(Tree::Node* n) {
        if (n->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" 
        || n->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" ) {
            string procedure_name = n->children[1]->lexema;
            generateCodeWithName(n, procedure_name);
        }
        else {
            for (Tree::Node* n: n->children) {
                generateCode(n);
            }
        }
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
            cerr << it->first << ":";
            for (string type: it->second.first) {
                cerr << " " << typeString(type);
            }
            cerr << endl;
            for (SymbolTable:: iterator it2 = it->second.second.begin(); it2 != it->second.second.end(); ++it2) {
                cerr << it2->first << " ";
                cerr << typeString((*it2).second.first) << " ";
                cerr << (*it2).second.second;
                cerr << endl;
            }
        }
    }
    
    void updateExistingOffsets(string procedure_name) {
        map<string, pair<string, int>>::iterator it;
        for (it = procedure_table[procedure_name].second.begin(); it != procedure_table[procedure_name].second.end(); ++it) {
            (*it).second.second += 4;
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
        } else if (n->rule == "dcl type ID") {
            string parameter_name = n->children[1]->lexema;
            if (checkExistingSymbol(procedure_name, parameter_name)) {
                cerr << "ERROR: already declared varaible" << endl;
            } else {
                if (n->children[0]->rule == "type INT STAR") {
                    string parameter_type = "INT STAR";
                    // updateExistingOffsets(procedure_name);
                    pair<string, pair<string, int>> symbol_row = make_pair(parameter_name, make_pair(parameter_type, this->offsets[procedure_name]));
                    this->offsets[procedure_name] -= 4;
                    this->procedure_table[procedure_name].second.insert(symbol_row);
                } else if (n->children[0]->rule == "type INT") {
                    string parameter_type = "INT";
                    // updateExistingOffsets(procedure_name);
                    pair<string, pair<string, int>> symbol_row = make_pair(parameter_name, make_pair(parameter_type, this->offsets[procedure_name]));
                    this->offsets[procedure_name] -= 4;
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
                this->offsets.insert(pair<string, int>(procedure_name, 0));
            }
        } else {
            for (Tree::Node* n: n->children) {
                buildProcedureTables(n);
            }
        }     
    }

    string getType(string procedure_name, string symbol_name) {
        return this->procedure_table[procedure_name].second[symbol_name].first;
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
    // t.checkTypeWithProcedureName(t.root, "do not care");
    t.generateCode(t.root);
    return 1;
}