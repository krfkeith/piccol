
#include <iostream>
#include <fstream>
#include <streambuf>

#include "macrolan.h"


int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <macrolan.metal>" << std::endl;
        return 1;
    }

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(std::cin),
               std::istreambuf_iterator<char>());


    macrolan::Macrolan m(code);

    std::string ret = m.parse(inp);

    std::cout << ret << std::endl;

    return 0;
}
