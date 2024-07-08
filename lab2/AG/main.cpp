// Project Identifier : 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include <iostream>
#include <stack>
#include <iterator>
#include <vector>
using namespace std;

int main() {
    istream_iterator<char> eos;
    istream_iterator<char> iit (cin);
    stack<char> brackets;

    while(iit != eos){
        if(*iit == '(' || *iit == '[' || *iit == '{'){
            brackets.push(*iit);
        }
        else if(*iit == ')'){
            if(brackets.empty()){
                cout << "Not balanced" << endl;
                return 0;
            }
            else if(brackets.top() == '('){
                brackets.pop();
            }
            else{
                cout << "Not balanced" << endl;
                return 0;
            }
        }
        else if(*iit == ']'){
            if(brackets.empty()){
                cout << "Not balanced" << endl;
                return 0;
            }
            else if(brackets.top() == '['){
                brackets.pop();
            }
            else{
                cout << "Not balanced" << endl;
                return 0;
            }
        }
        else if(*iit == '}'){
            if(brackets.empty()){
                cout << "Not balanced" << endl;
                return 0;
            }
            else if(brackets.top() == '{'){
                brackets.pop();
            }
            else{
                cout << "Not balanced" << endl;
                return 0;
            }
        }
        iit++;
    }

    if(brackets.size() == 0){
        cout << "Balanced" << endl;
    }
    else{
        cout << "Not balanced" << endl;
    }
    return 0;

}