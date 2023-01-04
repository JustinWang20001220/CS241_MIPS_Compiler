#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <sstream>

using namespace std;

bool test = false;

struct DFA
{
    string current_state;
    vector<string> alphabet;
    vector<string> states;
    string initial_state;
    vector<string> accepting_states;
    map<pair<string, string>, string> transistions;

    bool transit(string input)
    {
        pair<string, string> key = pair<string, string>(this->current_state, input);
        map<pair<string, string>, string>::iterator it = this->transistions.find(key);
        if (it != this->transistions.end()) // no such transistion is found
        {
            this->current_state = it->second;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool check_accepting()
    {
        for (string accepting_state: this->accepting_states)
        {
            if (accepting_state == this->current_state)
            {
                return true;
            }
        }
        return false;
    }
};

int main()
{
    DFA dfa_obj;
    int input_num;
    string input_string;

    cin >> input_num; // read in number of alphabet
    if (test)
    {
        cout << "number of alphabet: " << input_num << endl;
        cout << "-----------------------" << endl;
    }

    for (int i = 0; i < input_num; ++i) 
    {   
        cin >> input_string; // read in a new alphabet
        dfa_obj.alphabet.push_back(input_string);
    }

    cin >> input_num; // read in number of states
    if (test)
    {
        cout << "number of states: " << input_num << endl;
        cout << "-----------------------" << endl;
    }
    for (int i = 0; i < input_num; ++i)
    {
        cin >> input_string; // read in a new state
        dfa_obj.states.push_back(input_string);
    }

    cin >> input_string; // read in initial states
    dfa_obj.initial_state = input_string;
    if (test)
    {
        cout << "initial state: " << input_string << endl;
        cout << "-----------------------" << endl;
    }

    cin >> input_num; // read in number of accepting states
    if (test)
    {
        cout << "number of accepting states: " << input_num << endl;
        cout << "-----------------------" << endl;
    }
    for (int i = 0; i < input_num; ++i)
    {
        cin >> input_string;
        dfa_obj.accepting_states.push_back(input_string);
    }

    cin >> input_num; // read in number of transistion procedures
    if (test)
    {
        cout << "number of transistion rules: " << input_num << endl;
        cout << "-----------------------" << endl;
    }
    string transistion_from;
    string read_in_alphabet;
    string transistion_to;
    for (int i = 0; i < input_num; ++i)
    {
        cin >> transistion_from;
        cin >> read_in_alphabet;
        cin >> transistion_to;
        pair<string, string> new_key = pair<string, string>(transistion_from, read_in_alphabet);
        dfa_obj.transistions[new_key] = transistion_to;
    }

    // reading the input string line
    string lineStr;
    getline(cin, lineStr); // somehow ignore the current line
    while (getline(cin, lineStr))
    {
        // initializing the dfa object
        dfa_obj.current_state = dfa_obj.initial_state;
        if (test)
        {
            cout << lineStr << endl;
        }

        // read in the new line input
        string new_symbol;
        stringstream line(lineStr);
        bool interupted = false;
        while (line >> new_symbol)
        {
            bool result = dfa_obj.transit(new_symbol);
            if (result == false)
            {
                cout << "false" << endl;
                interupted = true;
                break;
            }
        }

        if (interupted == false)
        {
            if (dfa_obj.check_accepting() == true)
            {
                cout << "true" << endl;
            }
            else
            {
                cout << "false" << endl;
            }
        }
    }
}