
#include <random>

#include "piccol_mod.h"



bool do_cout_int(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                 const nanom::Struct& struc, nanom::Struct& ret) {

    std::cout << struc.v[0].inte;
    return true;
}

bool do_cout_sym(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                 const nanom::Struct& struc, nanom::Struct& ret) {

    std::cout << metalan::symtab().get(struc.v[0].uint);
    return true;
}

bool do_cout_uint(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                 const nanom::Struct& struc, nanom::Struct& ret) {

    std::cout << struc.v[0].uint;
    return true;
}

bool do_cout_real(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                  const nanom::Struct& struc, nanom::Struct& ret) {

    std::cout << struc.v[0].real;
    return true;
}

bool do_cin_int(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                const nanom::Struct& struc, nanom::Struct& ret) {
    nanom::Int v;
    std::cin >> v;
    ret.v.push_back(v);
    return true;
}

bool do_cin_uint(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                 const nanom::Struct& struc, nanom::Struct& ret) {
    nanom::UInt v;
    std::cin >> v;
    ret.v.push_back(v);
    return true;
}

bool do_cin_real(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                 const nanom::Struct& struc, nanom::Struct& ret) {
    nanom::Real v;
    std::cin >> v;
    ret.v.push_back(v);
    return true;
}

bool do_cin_sym(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                const nanom::Struct& struc, nanom::Struct& ret) {
    std::string v;
    std::getline(std::cin, v);
    nanom::UInt v2 = metalan::symtab().get(v);
    ret.v.push_back(v2);
    return true;
}

struct _rnd {
    std::mt19937 gen;
    _rnd() {
        gen.seed(::time(NULL));
    }
};

bool do_unirand(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
                const nanom::Struct& struc, nanom::Struct& ret) {
    static _rnd r;
    std::uniform_int_distribution<nanom::Int> dist(struc.v[0].inte, struc.v[1].inte);
    ret.v.push_back(dist(r.gen));
    return true;
}

int main(int argc, char** argv) {

    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <sysdir> <appdir> <modspec> <funname> <funrettype>" << std::endl;
        return 1;
    }

    piccol::Modules mod(argv[1], argv[2], argv[3]); 

    mod.register_callback("print", "Int", "Void", do_cout_int);
    mod.register_callback("print", "Sym", "Void", do_cout_sym);
    mod.register_callback("print", "UInt", "Void", do_cout_uint);
    mod.register_callback("print", "Real", "Void", do_cout_real);
    mod.register_callback("print", "Bool", "Void", do_cout_uint);

    mod.register_callback("read", "Void", "Int", do_cin_int);
    mod.register_callback("read", "Void", "Sym", do_cin_sym);
    mod.register_callback("read", "Void", "UInt", do_cin_uint);
    mod.register_callback("read", "Void", "Real", do_cin_real);
    mod.register_callback("read", "Void", "Bool", do_cin_uint);

    mod.register_callback("random", "[ Int Int ]", "Int", do_unirand);
    
    nanom::Struct out;
    nanom::Struct inp;

    bool ret = mod.run(argv[4], "Void", argv[5], inp, out);

    if (!ret) {
        std::cout << "fail." << std::endl;
    } else {
        std::cout << "ok." << std::endl;
    }

    return 0;
}
