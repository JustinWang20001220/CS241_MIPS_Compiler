#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;

bool test = true;

bool operator==(vector<string> lhs, vector<string> rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (int i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

struct Node {
    pair<string, string> content;
    vector<Node*> children;

    Node(pair<string, string> content, vector<Node*> children) {
        this->content = content;
        this->children = children;
    }

    ~Node() {
        for (Node* n: children) {
            delete n;
        }
    }

    void in_order_print() {
        if (this->children.empty()) {
            if (this->content.second != "useless") {
                cout << this->content.first << " " << this->content.second << endl;
            } else {
                cout << this->content.first << endl;
            }
            return;
        } else {
            cout << this->content.first;
            for (Node* item: this->children) {
                cout << " " << item->content.first;
            }
            cout << endl;
            for (Node* item: this->children) {
                item->in_order_print();
            }
        }
    }
};

struct ProductionRule {
    string lhs; 
    vector<string> rhs;
    string og_string;

    ProductionRule(string lhs, vector<string> rhs, string og_string) {
        this->lhs = lhs;
        this->rhs = rhs;
        this->og_string = og_string;
    }

    void print_production_rule() {
        cout << this->og_string << endl;
    }

    ~ProductionRule() {}
};

struct ShiftReduce {
    int from; // represent current state index
    string readin;
    bool is_reduce;
    int to; // to can represent production rule index or state index
    ShiftReduce(int from, string readin, bool is_reduce, int to) {
        this->from = from;
        this->readin = readin;
        this->is_reduce = is_reduce;
        this->to = to;
    }
    ~ShiftReduce() {};
};

struct BottomUpParsing {
    vector<string> non_terminals;
    vector<string> terminals;
    vector<ProductionRule> production_rules;
    vector<ShiftReduce> shift_reduce_laws;
    vector<pair<string, string>> look_up_table;
    string start_state;
    int current_state;
    
    BottomUpParsing(vector<string> terminals, vector<string> non_terminals, 
    string start_state, vector<ProductionRule> production_rules, vector<ShiftReduce> shift_reduce_laws) {
        this->terminals = terminals;
        this->non_terminals = non_terminals;
        this->production_rules = production_rules;
        this->shift_reduce_laws = shift_reduce_laws;
        this->start_state = start_state;
    }

    ~BottomUpParsing() {}

    vector<Node*> tokenize() {
        vector<Node*> result;
        Node* BOF_node_ptr = new Node(pair<string, string>("BOF", "BOF"), {});
        result.push_back(BOF_node_ptr);
        string temp_word;
        string temp_word2;
        while(cin >> temp_word) {
            cin >> temp_word2;
            Node* new_node_ptr = new Node(pair<string, string>(temp_word, temp_word2), {});
            result.push_back(new_node_ptr);
        }
        Node* EOF_node_ptr = new Node(pair<string, string>("EOF", "EOF"), {});
        result.push_back(EOF_node_ptr);
        return result;
    }

    void print_lexeme(pair<string, string> terminal) {
        cout << terminal.first << " " << terminal.second << endl;
    }

    bool is_terminal(string token) {
        for (string s: terminals) {
            if (token == s) {
                return true;
            }
        }
        return false;
    }

    void error_printing(int position) {
        cerr << "ERROR at " << position << endl;
    }

    pair<bool, int> search_shift_reduce(int current_state, string current_symbol) {
        for (int i = 0; i < this->shift_reduce_laws.size(); ++i) {
            if (shift_reduce_laws[i].from == current_state &&
                shift_reduce_laws[i].readin == current_symbol) {
                return pair<bool, int>(shift_reduce_laws[i].is_reduce, shift_reduce_laws[i].to);
            }
        }
        return pair<bool, int>(false, -1);
    }

    void parsing() {
        this->current_state = 0;
        vector<Node*> token_remained = tokenize();
        int og_length = token_remained.size();
        reverse(token_remained.begin(), token_remained.end());
        vector<Node*> parse_stack;
        vector<int> state_stack;
        state_stack.push_back(0);
        while (!token_remained.empty()) {
            Node* next_symbol = token_remained.back();
            pair<bool, int> search_result = search_shift_reduce(current_state, next_symbol->content.first);
            if (search_result.second == -1) { // no such transistion
                error_printing(og_length - token_remained.size());
                vector<Node*> deleted_nodes;
                // production_rule_used.print_production_rule();
                int og_size = parse_stack.size();
                // cout << parse_stack.size() << endl;
                for (int i = 0; i < og_size; ++i) {
                    // cout << "index i: " << i << endl;
                    Node* last_node_ptr = parse_stack.back();
                    deleted_nodes.insert(deleted_nodes.begin(), last_node_ptr);
                    state_stack.pop_back();
                    parse_stack.back() = nullptr; // prevent from deleting
                    parse_stack.pop_back();
                }
                int og_size2 = token_remained.size();
                // cout << parse_stack.size() << endl;
                for (int i = 0; i < og_size2; ++i) {
                    // cout << "index i: " << i << endl;
                    Node* last_node_ptr = token_remained.back();
                    deleted_nodes.insert(deleted_nodes.begin(), last_node_ptr);
                    token_remained.back() = nullptr; // prevent from deleting
                    token_remained.pop_back();
                }
                // cout << deleted_nodes.size() << endl;
                Node* final_node = new Node(pair<string, string>("shit", "useless"), deleted_nodes);
                delete final_node;
                return;
            } else {
                if (search_result.first == false) { // shifting
                    parse_stack.push_back(next_symbol);
                    // if (is_terminal(next_symbol.first)) {
                    //     print_lexeme(next_symbol);
                    // }
                    token_remained.pop_back();
                    state_stack.push_back(search_result.second);
                    this->current_state = search_result.second;
                } else { // need reduce
                    ProductionRule production_rule_used = this->production_rules[search_result.second];
                    vector<Node*> deleted_nodes;
                    // production_rule_used.print_production_rule();
                    for (int i = 0; i < production_rule_used.rhs.size(); ++i) {
                        Node* last_node = parse_stack.back();
                        deleted_nodes.insert(deleted_nodes.begin(), last_node);
                        state_stack.pop_back();
                        parse_stack[parse_stack.size() - 1] = nullptr; // prevent from deleting
                        parse_stack.pop_back();
                    }
                    Node* added_node_ptr = new Node(pair<string, string>(production_rule_used.lhs, "useless"), deleted_nodes);
                    this->current_state = (int) state_stack.back();
                    token_remained.push_back(added_node_ptr);
                }
            }
        }
        vector<string> new_parse_stack_end;
        for (Node* item: parse_stack) {
            new_parse_stack_end.push_back(item->content.first);
        }
        bool find_last_production = false;
        for (ProductionRule production_rule: production_rules) {
            if (production_rule.rhs == new_parse_stack_end) {
                find_last_production = true;
                vector<Node*> deleted_nodes;
                // production_rule_used.print_production_rule();
                int og_size = parse_stack.size();
                // cout << parse_stack.size() << endl;
                for (int i = 0; i < og_size; ++i) {
                    // cout << "index i: " << i << endl;
                    Node* last_node_ptr = parse_stack.back();
                    deleted_nodes.insert(deleted_nodes.begin(), last_node_ptr);
                    state_stack.pop_back();
                    parse_stack.back() = nullptr; // prevent from deleting
                    parse_stack.pop_back();
                }
                // cout << deleted_nodes.size() << endl;
                Node* final_node = new Node(pair<string, string>(production_rule.lhs, "useless"), deleted_nodes);
                final_node->in_order_print();
                delete final_node;
                return;
            }
        }
        if (find_last_production == false) {
            error_printing(og_length - token_remained.size());
            parse_stack.clear();
            token_remained.clear();
        }

    }
};

int main() {
    // initialize all the necessary values
    ifstream wlp4_rule("wlp4rule.lr1");
    int terminal_count;
    vector<string> terminals;
    int non_terminal_count;
    vector<string> non_terminals;
    string start_state;
    int production_rule_count;
    vector<ProductionRule> production_rules;
    int state_count;
    int shift_reduce_count;
    vector<ShiftReduce> shift_reduce_laws;

    // reading/storing terminals
    wlp4_rule >> terminal_count;
    string temp_terminal;
    for (int i = 0; i < terminal_count; ++i) {
        wlp4_rule >> temp_terminal;
        if (false) {
            cout << temp_terminal << endl;
        }
        terminals.push_back(temp_terminal);
    }
    // reading/storing non_terminals
    wlp4_rule >> non_terminal_count;
    string temp_non_terminal;
    for (int i = 0; i < non_terminal_count; i++) {
        wlp4_rule >> temp_non_terminal;
        if (false) {
            cout << temp_non_terminal << endl;
        }
        non_terminals.push_back(temp_non_terminal);
    }
    // reading/storing starting state
    wlp4_rule >> start_state;
    // reading/storing production rules
    wlp4_rule >> production_rule_count;
    if (false) {
        cout << "production rule count: " << production_rule_count << endl;
    }
    string production_rule_string;
    getline(wlp4_rule, production_rule_string);
    for (int i = 0; i < production_rule_count; ++i) {
        getline(wlp4_rule, production_rule_string);
        if (false) {
            cout << production_rule_string << endl;
        }
        stringstream production_rule_ss(production_rule_string);
        string lhs;
        production_rule_ss >> lhs;
        string temp_rhs_symbol;
        vector<string> rhs;
        while (production_rule_ss >> temp_rhs_symbol) {
            rhs.push_back(temp_rhs_symbol);
        }
        ProductionRule new_production_rule(lhs, rhs, production_rule_string);
        production_rules.push_back(new_production_rule);
    }
    // reading/storing shifting/reducing laws
    wlp4_rule >> shift_reduce_count;
    wlp4_rule >> shift_reduce_count;
    string shift_reduce_law_s;
    getline(wlp4_rule, shift_reduce_law_s);
    for (int i = 0; i < shift_reduce_count; ++i) {
        getline(wlp4_rule, shift_reduce_law_s);
        if (false) {
            cout << shift_reduce_law_s << endl;
        }
        stringstream shift_reduce_law_ss(shift_reduce_law_s);
        int from;
        string readin;
        string reduce_or_shift;
        bool is_reduce;
        int to;
        shift_reduce_law_ss >> from;
        shift_reduce_law_ss >> readin;
        shift_reduce_law_ss >> reduce_or_shift;
        if (reduce_or_shift == "reduce") {
            is_reduce = true;
        } else {
            is_reduce = false;
        }
        shift_reduce_law_ss >> to;
        ShiftReduce new_shift_reduce_law(from, readin, is_reduce, to);
        shift_reduce_laws.push_back(new_shift_reduce_law);
    }
    BottomUpParsing bottom_up_parser(terminals, non_terminals, start_state, production_rules, shift_reduce_laws);
    bottom_up_parser.parsing();
}