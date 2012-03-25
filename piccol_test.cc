
#include <random>

#include "piccol_vm.h"

void print_(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Struct& struc) {

    std::cout << "{" << std::endl;
    for (const auto& i : shape.sym2field) {

        std::cout << metalan::symtab().get(i.first) << "=";

        switch (i.second.type) {
        case nanom::BOOL:
        case nanom::INT:
            std::cout << struc.get_field(i.second.ix_from).inte;
            break;
        case nanom::UINT:
            std::cout << struc.get_field(i.second.ix_from).uint;
            break;
        case nanom::REAL:
            std::cout << struc.get_field(i.second.ix_from).real;
            break;
        case nanom::SYMBOL:
            std::cout << metalan::symtab().get(struc.get_field(i.second.ix_from).uint);
            break;
        case nanom::STRUCT:
            print_(shapes, shapes.get(i.second.shape), struc.substruct(i.second.ix_from, i.second.ix_to));
            break;
        case nanom::NONE:
            std::cout << "<fail>";
            break;
        }

        std::cout << std::endl;
    }
    std::cout << "}" << std::endl;
}


bool printer(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Shape& shapeto,
             const nanom::Struct& struc, nanom::Struct& ret) {
    print_(shapes, shape, struc);
    return true;
}

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

    std::string inp;

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <file> <funname> <funrettype>" << std::endl;
        return 1;
    }

    if (argv[1] == std::string("-")) {

        inp.assign(std::istreambuf_iterator<char>(std::cin),
                   std::istreambuf_iterator<char>());

    } else {
        
        std::ifstream ifile(argv[1]);

        if (!ifile)
            throw std::runtime_error("Could not open '" + std::string(argv[1]) + "'");

        inp.assign(std::istreambuf_iterator<char>(ifile),
                   std::istreambuf_iterator<char>());
    }

    piccol::Piccol l(piccol::load_file("macrolan.metal"),
                     piccol::load_file("piccol_lex.metal"),
                     piccol::load_file("piccol_morph.metal"),
                     piccol::load_file("piccol_emit.metal"),
                     piccol::load_file("prelude.piccol"));

    l.init();

    l.register_callback("print", "Int", "Void", do_cout_int);
    l.register_callback("print", "Sym", "Void", do_cout_sym);
    l.register_callback("print", "UInt", "Void", do_cout_uint);
    l.register_callback("print", "Real", "Void", do_cout_real);
    l.register_callback("print", "Bool", "Void", do_cout_uint);

    l.register_callback("read", "Void", "Int", do_cin_int);
    l.register_callback("read", "Void", "Sym", do_cin_sym);
    l.register_callback("read", "Void", "UInt", do_cin_uint);
    l.register_callback("read", "Void", "Real", do_cin_real);
    l.register_callback("read", "Void", "Bool", do_cin_uint);

    l.register_callback("random", "[ Int Int ]", "Int", do_unirand);
    
    l.load(inp);

    nanom::Struct out;

    bool ret = l.run(argv[2], "Void", argv[3], out);

    if (!ret) {
        std::cout << "fail." << std::endl;
    } else {
        print_(l.vm.shapes, l.vm.shapes.get(argv[3]), out);
    }

    return 0;
}
