
#include <iostream>
#include "cromlech.h"

std::string token_name(int type) {
    switch (type) {
    case crom::LITERAL: return "LITERAL";
    case crom::TUPLE_ELEMENT: return "TUPLE_ELEMENT";
    case crom::TUPLE_CLONE: return "TUPLE_CLONE";
    case crom::STRUCT_KEY: return "STRUCT_KEY";
    case crom::STRUCT_VAL: return "STRUCT_VAL";
    case crom::TUPLE_LITERAL: return "TUPLE_LITERAL";
    case crom::STRUCT_LITERAL: return "STRUCT_LITERAL";
    case crom::SYMBOL_TYPE: return "SYMBOL_TYPE";
    case crom::INT_TYPE: return "INT_TYPE";
    case crom::REAL_TYPE: return "REAL_TYPE";
    case crom::BOOL_TYPE: return "BOOL_TYPE";
    case crom::STRING_TYPE: return "STRING_TYPE";
    case crom::CUSTOM_TYPE: return "CUSTOM_TYPE";
    case crom::TUPLE_TYPE: return "TUPLE_TYPE";
    case crom::STRUCT_TYPE: return "STRUCT_TYPE";
    case crom::MATCH_TUPLE: return "MATCH_TUPLE";
    case crom::MATCH_STRUCT: return "MATCH_STRUCT";
    case crom::VARDEF: return "VARDEF";
    case crom::VARGET: return "VARGET";
    case crom::TUPLE_START: return "TUPLE_START";
    case crom::TUPLE_END: return "TUPLE_END";
    case crom::STRUCT_START: return "STRUCT_START";
    case crom::STRUCT_END: return "STRUCT_END";
    default: return "?";
    }
}

struct printer {

    void operator()(const crom::Int& t) { std::cout << "Int:" << t; }
    void operator()(const crom::Real& t) { std::cout << "Real:" << t; }
    void operator()(const crom::Bool& t) { std::cout << "Bool:" << t; }

    void operator()(const crom::Symbol& t) { 
        if (t == 0) {
            std::cout << "-";
        } else {
            std::cout << "Symbol:" << t 
                      << " (" << crom::Singleton<crom::SymTable>().get(t) << ")";
        }
    }

    void operator()(const crom::String& t) { std::cout << "String:'" << t << "'" << std::endl; }
};


void print(const crom::PODLiteral& l) {
    l(printer());
}

template <typename STACK>
void print_stack(const STACK& s, const std::string& indent = "") {

    for (auto t : s) {
        std::cout << indent << token_name(t->token.type) << ": ";
        print(t->token.val);
        std::cout << std::endl;

        print_stack(t->edges, indent + "    ");
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
