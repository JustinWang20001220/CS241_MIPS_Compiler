#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Node {
    public:
    int val;
    int child_count;
    vector<Node*> children;

    Node() {
        cin >> this->val;
        cin >> this->child_count;
        for (int i = 0; i < child_count; ++i) {
            Node* new_node = new Node();
            this->children.emplace_back(new_node);
        }
    }

    ~Node() {
        for (Node* node: this->children) {
            delete node;
        }
    }

    void post_order_traversal() {
        for(int i = 0; i < this->children.size(); ++i){
            this->children[i]->post_order_traversal();    
        }
        cout << this->val << " " << this->child_count << endl;
    }
};

int main() {
    Node* root = new Node();
    root->post_order_traversal();
    delete root;
    return 0;
}