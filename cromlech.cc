
#include <iostream>
#include "cromlech.h"


int main(int argc, char** argv) {

  try {
    if (argc > 2) {
      pegtl::trace_parse_string<crom::tunit>(true, pegtl::read_string(argv[1]));

    } else {
      pegtl::basic_parse_string<crom::tunit>(pegtl::read_string(argv[1]));
    }

  } catch (std::runtime_error& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
