
#include <iostream>
#include <fstream>
#include <streambuf>

#include "picolol_vm.h"


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

    picolol::Picolol l;

    std::ifstream lfile("picolol_lex.metal");
    std::ifstream pfile("picolol_parse.metal");

    if (!lfile)
        throw std::runtime_error("Could not open 'picolol_lex.metal'");

    if (!pfile)
        throw std::runtime_error("Could not open 'picolol_parse.metal'");

    std::string lexer;
    std::string parser;

    lexer.assign(std::istreambuf_iterator<char>(lfile),
                 std::istreambuf_iterator<char>());

    parser.assign(std::istreambuf_iterator<char>(pfile),
                  std::istreambuf_iterator<char>());

    l.load(lexer, parser, inp);

    return 0;
}
