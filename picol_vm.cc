
#include <iostream>
#include <fstream>
#include <streambuf>

#include "picol_vm.h"


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

    l.load(lexer, morpher, emiter, inp);

    return 0;
}
