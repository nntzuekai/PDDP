#include <iostream>
#include <fstream>
#include "PDDP_tree.h"

using namespace std;
using namespace nzk;


int main(){
    ifstream f1(R"(D:\Code\DB\competitor\D_codebook.bit)",ifstream::binary);
//    ifstream f1(R"(D:\Code\DB\competitor\prob_codebook.bit)",ifstream::binary);

    decltype(PDDP<>::size) sz;
    f1.read(reinterpret_cast<char *>(&sz), sizeof(sz));
    vector<pair<double, vector<unsigned char>>> v1(sz);
    map<double,double> clu1;

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
    f1.read(reinterpret_cast<char *>(&sz), sizeof(sz));
    for(unsigned i=0;i<sz;++i){
        double val1,val2;
        f1.read(reinterpret_cast<char *>(&val1), sizeof(val1));
        f1.read(reinterpret_cast<char *>(&val2), sizeof(val2));
        clu1.emplace(val1,val2);
    }


    PDDP<> t1(v1,std::move(clu1));
    for(const auto &[v,c]:v1){
        cout<<v<<',';
        for(bool b:t1.code_of(v)){
            cout<<b;
        }
        cout<<'\n';
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
