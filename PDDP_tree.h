//
// Created by Arche on 2019/6/2.
//

#ifndef PDDP_PDDP_TREE_H
#define PDDP_PDDP_TREE_H

#include <optional>
#include <vector>
#include <memory>
#include <initializer_list>
#include <cmath>
#include <map>
#include <set>
#include <utility>
#include <string_view>

namespace nzk{
    template <typename T=double>struct PDDP;

    template <typename T=double>
    struct PDDP_node{
        PDDP_node *left= nullptr;
        PDDP_node *right= nullptr;
        std::optional<T> val;

        PDDP_node(PDDP_node *left= nullptr, PDDP_node *right= nullptr):left(left), right(right){}
        PDDP_node(const T &val, PDDP_node *left= nullptr, PDDP_node *right=nullptr):left(left),right(right),val(val){}

        PDDP_node &operator=(PDDP_node &&src) noexcept = default;
        PDDP_node &operator=(const PDDP_node &src)= default;
        void delete_children();

        friend PDDP<T>;
    };

    template<typename T>
    void PDDP_node<T>::delete_children() {
        if(left){
            delete left;
        }
        if(right){
            delete right;
        }
    }

    template <typename T>
    using PDDP_codebook=std::vector<std::pair<T,std::vector<bool>>>;

    template <typename T>
    struct PDDP{
        using node_type=PDDP_node<T>;
        node_type root_node;
        unsigned size=0;
//        std::map<T,std::vector<bool>> code;

        static constexpr double ita=1;
        static constexpr double e=500;
        static constexpr double error_bound=ita/std::abs(e);


        PDDP()= default;
        PDDP(std::initializer_list<T> args);

        template <typename Container>
        explicit PDDP(const Container &src);

        virtual ~PDDP();

        void DDP_insert(const T &t);
        std::vector<bool> code(const T &t)const;
//        void DDP_expand(node_type *node);
        static void check_prunned(node_type *p);

        std::size_t decode(std::vector<T> &tar,std::string_view code);

        PDDP_codebook<T> codebook()const;

        void _make_codebook(PDDP_codebook<T> &book,const node_type *pnode,std::vector<bool> v={}) const;
    };

    template<typename T>
    PDDP<T>::PDDP(std::initializer_list<T> args) {
        for(const auto &val:args){
            DDP_insert(val);
        }

        check_prunned(&root_node);
    }

    template<typename T>
    void PDDP<T>::DDP_insert(const T &t) {
        auto *p=&root_node;
//        std::vector<bool > rep;
        T rem=t;
        T alpha_base=1;

        T rp=0;
        T err=error_bound;
        while (rem>err){
            while (rem*2<1){
                rem*=2;
                err*=2;
//                rep.push_back(0);
                alpha_base/=2;

                if(p->left== nullptr){
                    p->left=new node_type;
                }
                else if(p->left->val.has_value()&&p->left->val!=t){
                    auto q=p->left;
                    p->left=new node_type{q};

//                    code.at(q->value).push_back(0);
                }

                p=p->left;
            }

            rem-=0.5;
            rp+=alpha_base;
            alpha_base/=2;
            rem*=2;
            err*=2;

//            rep.push_back(1);
            if(p->right== nullptr){
                p->right=new node_type;
            }
            else if(p->right->val.has_value()&&p->right->val!=t){
                auto q=p->right;
                p->right=new node_type{q};
//                code.at(q->value).push_back(0);
            }

            p=p->right;
        }
        if(p==&root_node||p->left||p->right){
            while (p->left){
                p=p->left;
            }

            if(p->val.has_value()){
                p->val=t;
            }
            else{
                p->left=new node_type{t};
                ++size;
            }
        }
        else{
            if(!p->val.has_value()){
                ++size;
            }
            p->val=t;
        }



        /*for(const auto &i:rep){
            if(i== false){
                if(p->left== nullptr){
                    p->left=new node_type;
                }
                else if(!p->left->left&&!p->left->right){
                    auto q=p->left;
                    p->left=new node_type{-1,q};

                    code.at(q->value).push_back(0);
                }

                p=p->left;
            } else{
                if(p->right== nullptr){
                    p->right=new node_type;
                }
                else if(!p->right->left&&!p->right->right){
                    auto q=p->right;
                    p->right=new node_type{-1,q};
                    code.at(q->value).push_back(0);
                }

                p=p->right;
            }
        }*/


//        code.emplace(t,std::move(rep));
    }

    /*template<typename T>
    void PDDP<T>::DDP_expand(PDDP::node_type *node) {
        auto val=node->value;
        node->value=-1;
        auto p=node;
        auto &rep=code.at(val);
        while (p->left!= nullptr){
            p=p->left;
            rep.push_back(0);
        }

        p->left=new node_type{val};
        rep.push_back(0);
    }*/

    template<typename T>
    void PDDP<T>::check_prunned(PDDP::node_type *p) {
        if(p->left&&p->right){
            check_prunned(p->left);
            check_prunned(p->right);
        }
        else if(!p->left&&!p->right){
            return;
        }
        else if(p->left){
            auto q=p->left;
            *p=*q;
            delete q;
        }
        else{
            auto q=p->right;
            *p=*q;
            delete q;
        }
    }

    template<typename T>
    std::size_t PDDP<T>::decode(std::vector<T> &tar,std::string_view code){
        auto p=&root_node;
        size_t cnt=0;

        auto bg=code.data();
        auto ed=bg+code.size();
        for(auto cur=bg;;){
            if(p->val.has_value()){
                tar.push_back(p->val.value());
                p=&root_node;
                cnt=cur-bg;
            }

            if(cur==ed){
                break;
            }

            switch (*cur){
                case '0':{
                    if(p->left){
                        p=p->left;
                    } else{
                        return cnt;
                    }
                    break;
                }
                default:{
                    if(p->right){
                        p=p->right;
                    } else{
                        return cnt;
                    }
                }
            }
            ++cur;

        }

        return cnt;
    }

    template<typename T>
    PDDP_codebook<T> PDDP<T>::codebook() const {
        PDDP_codebook<T> book;
        book.reserve(size);

        _make_codebook(book,&root_node);
        return book;
    }

    template<typename T>
    template <typename Container>
    PDDP<T>::PDDP(const Container &s) {
        for(const auto &t:s){
            DDP_insert(t);
        }

        check_prunned(&root_node);
    }


    template <typename T>
    void PDDP<T>::_make_codebook(PDDP_codebook<T> &book,const node_type *pnode,std::vector<bool> v) const{
        if(pnode->val.has_value()){
            book.emplace_back(pnode->val.value(),std::move(v));
        }
        else{
            if(pnode->left&&pnode->right){
                v.push_back(false);
                _make_codebook(book,pnode->left,v);
                v.back()=true;
                _make_codebook(book,pnode->right,std::move(v));
            }
            else if(pnode->left){
                v.push_back(false);
                _make_codebook(book,pnode->left,std::move(v));
            }
            else{
                v.push_back(true);
                _make_codebook(book,pnode->right,std::move(v));
            }
        }
    }

    template<typename T>
    PDDP<T>::~PDDP() {
        root_node.delete_children();
    }

    template<typename T>
    std::vector<bool> PDDP<T>::code(const T &t) const {
        std::vector<bool> rtv;

        auto *p=&root_node;
//        std::vector<bool > rep;
        T rem=t;
        T alpha_base=1;

        T rp=0;
        T err=error_bound;
        while (rem>err){
            while (rem*2<1){
                rem*=2;
                err*=2;
//                rep.push_back(0);
                alpha_base/=2;

                if(p->left== nullptr){
                    rtv.clear();
                    return rtv;
                }

                p=p->left;
                rtv.push_back(false);
            }

            rem-=0.5;
            rp+=alpha_base;
            alpha_base/=2;
            rem*=2;
            err*=2;

//            rep.push_back(1);
            if(p->right== nullptr){
                rtv.clear();
                return rtv;
            }

            p=p->right;
            rtv.push_back(true);
        }
        if(p==&root_node||!p->val.has_value()){
            while (p->left){
                p=p->left;
                rtv.push_back(false);
            }
        }

        return rtv;
    }
}



#endif //PDDP_PDDP_TREE_H
