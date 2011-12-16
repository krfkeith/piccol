
#include <iostream>
#include "cromlech.h"

struct printer {

    void operator()(const crom::Int& t) { std::cout << "Int:" << t; }
    void operator()(const crom::Real& t) { std::cout << "Real:" << t; }
    void operator()(const crom::Bool& t) { std::cout << "Bool:" << t; }

    void operator()(const crom::Symbol& t) { 
        std::cout << "Symbol:" << t 
                  << " (" << crom::Singleton<crom::SymTable>().get(t) << ")";
    }

    void operator()(const crom::String& t) { std::cout << "String:'" << t << "'" << std::endl; }
};


void print(const crom::PODLiteral& l) {
    l(printer());
}

void print_stack(const crom::stack_t& s) {

    for (const crom::Token& t : s) {
        std::cout << t.type << ": ";
        print(t.val);
        std::cout << std::endl;
    }
}


int main(int argc, char** argv) {

  try {

      crom::stack_t stack;

      if (argc > 2) {
          pegtl::trace_parse_string<crom::tunit>(true, 
                                                 pegtl::read_string(argv[1]),
                                                 stack);

      } else {
          pegtl::basic_parse_string<crom::tunit>(pegtl::read_string(argv[1]),
                                                 stack);
      }

      print_stack(stack);

  } catch (std::runtime_error& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
