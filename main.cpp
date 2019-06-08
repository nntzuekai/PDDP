#include <iostream>
#include <fstream>
#include "PDDP_tree.h"

using namespace std;
using namespace nzk;


int main(){
//    ifstream f1(R"(D:\Code\DB\competitor\D_codebook.bit)",ifstream::binary);
    ifstream f1(R"(D:\Code\DB\competitor\prob_codebook.bit)",ifstream::binary);

    decltype(PDDP<>::size) sz;
    f1.read(reinterpret_cast<char *>(&sz), sizeof(sz));
    vector<pair<double, vector<unsigned char>>> v1(sz);

    for(unsigned i=0;i<sz;++i){
        double val;
        f1.read(reinterpret_cast<char *>(&val), sizeof(val));
        v1[i].first=val;

        decltype(sz) len;
        f1.read(reinterpret_cast<char *>(&len), sizeof(len));
        v1[i].second.resize(len);

        for(unsigned j=0;j<len;++j){
            v1[i].second[j]=f1.get();
        }
    }


    PDDP<> t1(v1);
    for(const auto &[v,c]:v1){
        if(t1.code_of(v).empty()){
            auto code=t1.code_of(v);
            cerr<<v<<endl;
        }
    }

//    string s;
//    cin>>s;
//    vector<double > v;
//    cout<<t2.decode(v,s)<<endl;
//    for(const auto &t:v){
//        cout<<t<<' ';
//    }
//    cout<<endl;
    return 0;
}
