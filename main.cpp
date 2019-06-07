#include <iostream>
#include "PDDP_tree.h"

using namespace std;
using namespace nzk;


int main(){
    PDDP<> t1={0,0.375,0.5,0.75};

    string s;
    while (cin>>s){
        cout<<t1.decode(s)<<endl;
    }

    return 0;
}
