
#include <iostream>
#include <fstream>
#include <streambuf>

#include "picol_vm.h"

void printer(const nanom::Shapes& shapes, const nanom::Shape& shape, const nanom::Struct& struc) {

    for (const auto& i : struc.v) {
        std::cout << "// " << metalan::symtab().get(i.uint) << std::endl;
    }

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
            printer(shapes, shapes.get(i.second.shape), 
                    struc.substruct(i.second.ix_from, i.second.ix_to));
            break;
        case nanom::NONE:
            std::cout << "<fail>";
            break;
        }

        std::cout << std::endl;
    }
    std::cout << "}" << std::endl;
}


int main(int argc, char** argv) {

    std::string inp;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
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

    picol::Picol l;

    std::ifstream lfile("picol_lex.metal");
    std::ifstream mfile("picol_morph.metal");
    std::ifstream efile("picol_emit.metal");

    if (!lfile)
        throw std::runtime_error("Could not open 'picol_lex.metal'");

    if (!mfile)
        throw std::runtime_error("Could not open 'picol_morph.metal'");

    if (!efile)
        throw std::runtime_error("Could not open 'picol_emit.metal'");

    std::string lexer;
    std::string morpher;
    std::string emiter;

    lexer.assign(std::istreambuf_iterator<char>(lfile),
                 std::istreambuf_iterator<char>());

    morpher.assign(std::istreambuf_iterator<char>(mfile),
                   std::istreambuf_iterator<char>());

    emiter.assign(std::istreambuf_iterator<char>(efile),
                  std::istreambuf_iterator<char>());

    l.register_callback("Feature", printer);
    
    l.load(lexer, morpher, emiter, inp);

    return 0;
}
