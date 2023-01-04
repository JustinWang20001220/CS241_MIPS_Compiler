#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

class Node {
    public:
    string val;
    vector<Node*> children;

    Node(string a) {
        if (a == "-" || a == "(" || a == ")" || a == "id" || a == "BOF" || a == "EOF") {
            this->val = a;
        } else {
            cin >> this->val;
            string RHS;
            getline(cin, RHS);
            stringstream ss(RHS);
            string new_node_string;
            while (ss >> new_node_string) {
                Node* new_node = new Node(new_node_string);
                this->children.emplace_back(new_node);
            }
        }
    }

    ~Node() {
        for (Node* node: this->children) {
            delete node;
        }
    }

    int eval() {
        if (this->val == "BOF") {
            return -2;
        } else if (this->val == "EOF") {
            return -3;
        } else if (this->val == "-") {
            return -4;
        } else if (this->val == "id") {
            return 42;
        } else if (this->val == "(") {
            return -5;
        } else if (this->val == ")") {
            return -6;
        } else if (this->val == "S") {
            return this->children[1]->eval();
        } else if (this->val == "expr") {
            if (this->children.size() == 3) {
                return this->children[0]->eval() - this->children[2]->eval();
            } else {
                return this->children[0]->eval();
            }
        } else if (this->val == "term") {
            if (this->children.size() == 3) {
                return this->children[1]->eval();
            } else {
                return this->children[0]->eval();
            }
        }
        return -100;
    }
};

int main() {
    string rubbish;
    for (int i = 0; i < 18; ++i) {
        getline(cin, rubbish);
    }
    Node* root = new Node("S");
    cout << root->eval() << endl;
    delete root;
    return 0;
}