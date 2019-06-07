//
// Created by Arche on 2019/6/2.
//

#ifndef PDDP_PDDP_TREE_H
#define PDDP_PDDP_TREE_H

#include <optional>
#include <vector>
#include <initializer_list>
#include <cmath>
#include <map>
#include <string_view>

namespace nzk{
    template <typename T=double>struct PDDP;

    template <typename T=double>
    struct PDDP_node{
        T value=NAN;
        PDDP_node *left= nullptr;
        PDDP_node *right= nullptr;
        std::optional<std::vector<bool>> tail={};

        PDDP_node(
                const T &value=-1,
                PDDP_node *left= nullptr, PDDP_node *right= nullptr, const std::optional<std::vector<bool>> &tail={})
        :
        value(value),
        left(left), right(right), tail(tail) {}

        PDDP_node &operator=(PDDP_node &&src) noexcept ;

        friend PDDP<T>;
    };

    template<typename T>
    PDDP_node<T> &PDDP_node<T>::operator=(PDDP_node &&src) noexcept {
        value=src.value;
        left=src.left;
        right=src.right;
        tail=std::move(src.tail);

        return *this;
    }

    template <typename T>
    struct PDDP{
        using node_type=PDDP_node<T>;
        node_type root_node;
//        std::map<T,std::vector<bool>> code;

        static constexpr double ita=1;
        static constexpr double e=500;
        static constexpr double error_bound=ita/std::fabs(e);


        PDDP()= default;
        PDDP(std::initializer_list<T> args);

        void DDP_insert(const T &t);
//        void DDP_expand(node_type *node);
        static void check_prunned(node_type *p);

        T decode(std::string_view code) const;
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
        if(t<=error_bound){
            root_node.left=new node_type{0};
//            code.emplace(0,std::move(std::vector<bool>{0}));
            return;
        }


        auto *p=&root_node;
//        std::vector<bool > rep;
        T rem=t;
        T alpha_base=1;

        T rp=0;
        while (rem>error_bound){
            while (rem*2<1){
                rem*=2;
//                rep.push_back(0);
                alpha_base/=2;

                if(p->left== nullptr){
                    p->left=new node_type;
                }
                else if(!p->left->left&&!p->left->right){
                    auto q=p->left;
                    p->left=new node_type{-1,q};

//                    code.at(q->value).push_back(0);
                }

                p=p->left;
            }

            rem-=0.5;
            rp+=alpha_base;
            alpha_base/=2;
            rem*=2;

//            rep.push_back(1);
            if(p->right== nullptr){
                p->right=new node_type;
            }
            else if(!p->right->left&&!p->right->right){
                auto q=p->right;
                p->right=new node_type{-1,q};
//                code.at(q->value).push_back(0);
            }

            p=p->right;
        }
        p->value=t;

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
            p->left=q->left;
            p->right=q->right;
            p->value=q->value;
            if(p->tail){
               p->tail->push_back(0);
            }
            else{
                p->tail.emplace(1,false);
            }

            delete q;
        }
        else{
            auto q=p->right;
            p->left=q->left;
            p->right=q->right;
            p->value=q->value;
            if(p->tail){
                p->tail->push_back(1);
            }
            else{
                p->tail.emplace(1,true);
            }

            delete q;
        }
    }

    template<typename T>
    T PDDP<T>::decode(std::string_view code) const{
        T val=0;
        T base=0.5;
        auto p=&root_node;
        bool tailed=false;

        for(auto bg=code.begin(),ed=code.end();bg!=ed;){
            if(p->tail){
                for(bool i:p->tail.value()){
                    if(bg==ed){
                        return -1;
                    }
                    if(i==(*bg!='0')){
                        if(i){
                            val+=base;
                        }
                        base/=2;

                        ++bg;
                    }
                    else{
                        return -1;
                    }
                }
                tailed= false;
                continue;
            }


            if(*bg=='0'){
                if(p->left){
                    p=p->left;
                    base/=2;
                    ++bg;
                } else{
                    return -1;
                }
            }
            else{
                if(p->right){
                    p=p->right;

                    val+=base;
                    base/=2;
                    ++bg;
                } else{
                    return -1;
                }
            }
            if(p->tail){
                tailed= true;
            }
        }

        if(p->left||p->right||tailed){
            return -1;
        }
        return val;
    }
}


#endif //PDDP_PDDP_TREE_H
