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



/* Some useful sequencers for text input/output and formatting. */

namespace {

struct __formatter {
    std::string buff;
};


inline void __fmt_int(__formatter& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    os.buff += piccol::int_to_string(struc.v[0].inte);
}

inline void __fmt_uint(__formatter& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    os.buff += piccol::uint_to_string(struc.v[0].uint);
}

inline void __fmt_real(__formatter& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    os.buff += piccol::real_to_string(struc.v[0].real);
}

inline void __fmt_sym(__formatter& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    os.buff += metalan::symtab().get(struc.v[0].uint);
}

inline bool __fmt_get(__formatter& os, const Shapes& shapes, const Shape& shapeto, Struct& ret) {
    ret.v.push_back(metalan::symtab().get(os.buff));
    return true;
}

struct __printer {};

inline void __print_int(__printer& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    std::cout << struc.v[0].inte;
}

inline void __print_uint(__printer& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    std::cout << struc.v[0].uint;
}

inline void __print_real(__printer& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    std::cout << struc.v[0].real;
}

inline void __print_sym(__printer& os, const Shapes& shapes, const Shape& shape, const Struct& struc) {
    std::cout << metalan::symtab().get(struc.v[0].uint);
}

inline bool __print_get_void(__printer& os, const Shapes& shapes, const Shape& shapeto, Struct& ret) {
    return true;
}

template <typename T>
inline bool __print_get_v(__printer& os, const Shapes& shapes, const Shape& shapeto, Struct& ret) {

    if (!std::cin) return false;

    T tmp;
    std::cin >> tmp;
    ret.v.push_back(tmp);
    return true;
}

inline bool __print_get_sym(__printer& os, const Shapes& shapes, const Shape& shapeto, Struct& ret) {

    if (!std::cin) return false;

    std::string v;
    std::getline(std::cin, v);
    nanom::UInt tmp = metalan::symtab().get(v);
    ret.v.push_back(tmp);
    return true;
}

}

template <typename VM>
void register_fmt_sequencer(VM& vm) {

    piccol::register_sequencer<__formatter>(vm, "fmt")
        .feed("Int", __fmt_int)
        .feed("UInt", __fmt_uint)
        .feed("Real", __fmt_real)
        .feed("Sym", __fmt_sym)
        .end("Sym", __fmt_get);

}

template <typename VM>
void register_print_sequencer(VM& vm) {

    piccol::register_sequencer<__printer>(vm, "print")
        .feed("Int", __print_int)
        .feed("UInt", __print_uint)
        .feed("Real", __print_real)
        .feed("Sym", __print_sym)
        .end("Void", __print_get_void)
        .end("Sym", __print_get_sym)
        .end("Int", __print_get_v<Int>)
        .end("UInt", __print_get_v<UInt>)
        .end("Real", __print_get_v<Real>);

}


}

#endif

