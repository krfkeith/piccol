#ifndef __PICCOL_SEQUENCERS_H
#define __PICCOL_SEQUENCERS_H

#include "piccol_vm.h"


namespace piccol {

template <typename T>
struct SeqHolder {

    std::vector<T> vstack;

    void push() { vstack.push_back(T()); }
    T& top() { return vstack.back(); }
    void pop() { vstack.pop_back(); }
};

template <typename T>
inline SeqHolder<T>& seqholder() {
    static SeqHolder<T> ret;
    return ret;
}

template <typename T, typename VM>
struct register_sequencer_ {

    VM& vm;
    std::string name;

    register_sequencer_(VM& _vm, const std::string& n) : vm(_vm), name(n) {
        vm.register_callback(name + "_start", "Void", "Void",
                             [](const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                                const Struct& struc, Struct& ret) {

                                 seqholder<T>().push();
                                 return true;
                             });
    }

    template <typename FUNC>
    register_sequencer_<T,VM>& feed(const std::string& intype, FUNC proc) {
        vm.register_callback(name, intype, "Void",
                             [proc](const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                                    const Struct& struc, Struct& ret) {

                                 proc(seqholder<T>().top(), shapes, shape, struc);
                                 return true;
                             });
        return *this;
    }

    template <typename FUNC>
    register_sequencer_<T,VM>& end(const std::string& outtype, FUNC proc) {
        vm.register_callback(name + "_end", "Void", outtype,
                             [proc](const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                                    const Struct& struc, Struct& ret) {

                                 bool r = proc(seqholder<T>().top(), shapes, shapeto, ret);
                                 seqholder<T>().pop();
                                 return r;
                             });
        return *this;
    }
};


template <typename T,typename VM>
register_sequencer_<T,VM> register_sequencer(VM& vm, const std::string& name) {
    return register_sequencer_<T,VM>(vm, name);
}

}

#endif

