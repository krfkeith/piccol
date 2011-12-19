
#include <iostream>
#include "cromlech.h"

std::string token_name(int type) {
    switch (type) {
    case crom::LITERAL: return "LITERAL";
    case crom::STRUCT_KEY: return "STRUCT_KEY";
    case crom::SYMBOL_TYPE: return "SYMBOL_TYPE";
    case crom::INT_TYPE: return "INT_TYPE";
    case crom::REAL_TYPE: return "REAL_TYPE";
    case crom::BOOL_TYPE: return "BOOL_TYPE";
    case crom::STRING_TYPE: return "STRING_TYPE";
    case crom::MATCH_TUPLE: return "MATCH_TUPLE";
    case crom::MATCH_STRUCT: return "MATCH_STRUCT";
    case crom::VARDEF: return "VARDEF";
    case crom::VARGET: return "VARGET";
    case crom::TUPLE_START: return "TUPLE_START";
    case crom::STRUCT_START: return "STRUCT_START";
    default: return "?";
    }
}

void print(const crom::Val&, const std::string&);

struct printer {

    const std::string& indent;
    printer(const std::string& i = "") : indent(i) {}

    void operator()(const crom::Int& t) { std::cout << indent << "Int:" << t << std::endl; }
    void operator()(const crom::Real& t) { std::cout << indent << "Real:" << t << std::endl; }
    void operator()(const crom::Bool& t) { std::cout << indent << "Bool:" << t << std::endl; }

    void operator()(const crom::Symbol& t) { 
        if (t == 0) {
            std::cout << indent << "-" << std::endl;
        } else {
            std::cout << indent << "Symbol:" << t 
                      << "=" << crom::Singleton<crom::SymTable>().get(t) << std::endl;
        }
    }

    void operator()(const crom::String& t) { 
	std::cout << indent << "String:'" << t << "'" << std::endl; 
    }

    void operator()(const crom::Val::stup_t& t) {
	for (const auto& v : *t) {
	    print(v, indent + "    ");
	}
    }	
};


void print(const crom::Val& l, const std::string& indent) {

    if (l.binding) {
        std::cout << indent << "(Binding: " << l.binding << "=" 
                  << crom::Singleton<crom::SymTable>().get(l.binding) << ")" << std::endl;
    }

    if (l.type == crom::Val::TUPLE) {
	std::cout << indent << "Tuple:" << std::endl;

    } else if (l.type == crom::Val::STRUCT) {
	std::cout << indent << "Struct:" << std::endl;

    } else if (l.type == crom::Val::TYPETAG) {
	std::cout << indent << "(Typetag)" << std::endl;
    }

    l(printer(indent));
}

template <typename ELT>
void print_element(const ELT& t, const std::string& indent = "") {

    std::cout << indent << token_name(t.type) << ": " << std::endl;
    print(t.val, indent);
}

template <typename STACK>
void print_context(const STACK& s, const std::string& indent = "") {

    std::cout << "--- Stack ---" << std::endl << std::endl;
    for (const auto& t : s.stack) {
	print_element(t);
    }
    std::cout << std::endl;

    std::cout << "--- Typemap ---" << std::endl << std::endl;

    for (const auto& v : s.typemap) {
	printer()(v.first.first);
	printer()(v.first.second);
	std::cout << "-->" << std::endl;
	print_element(v.second);
	std::cout << std::endl;
    }
}


int main(int argc, char** argv) {

  try {

      crom::Context stack;

      if (argc > 2) {
          pegtl::trace_parse_string<crom::tunit>(true, 
                                                 pegtl::read_string(argv[1]),
                                                 stack);

      } else {
          pegtl::basic_parse_string<crom::tunit>(pegtl::read_string(argv[1]),
                                                 stack);
      }

      print_context(stack);

  } catch (std::runtime_error& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
