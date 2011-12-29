
#include <iostream>
#include "cromlech.h"

std::string token_name(int type) {
    switch (type) {
    case crom::LITERAL: return "LITERAL";
    case crom::GET: return "GET";
    case crom::START_STRUCT: return "START_STRUCT";
    case crom::PUSH_STRUCT: return "PUSH_STRUCT";
    case crom::FRAME_GET: return "FRAME_GET";
    case crom::RETURN: return "RETURN";
    case crom::CLEAR: return "CLEAR";
    case crom::FUNCALL: return "FUNCALL";
    case crom::SYSCALL: return "SYSCALL";
    case crom::ADD: return "ADD";
    case crom::SUB: return "SUB";
    case crom::MUL: return "MUL";
    case crom::DIV: return "DIV";
    case crom::MOD: return "MOD";
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
        std::cout << indent << "(Binding: " << l.binding << ")" << std::endl;
    }

    if (l.type == crom::Val::STRUCT) {
	std::cout << indent << "Struct:" << std::endl;

    } else if (l.type == crom::Val::TYPETAG) {
	std::cout << indent << "(Typetag)" << std::endl;
    }

    l(printer(indent));
}

template <typename ELT>
void print_element(const ELT& t, const std::string& indent = "") {

    std::cout << indent << token_name(t.type);
    if (t.arg) {
        std::cout << "[" << t.arg << "]";
    }
    std::cout << ": " << std::endl;
    print(t.val, indent);
}

template <typename STACK>
void print_context(const STACK& s, const std::string& indent = "") {

    std::cout << "--- Stack ---" << std::endl << std::endl;
    for (const auto& t : s.vm.code) {
	print_element(t);
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "--- Typemap ---" << std::endl << std::endl;

    for (const auto& v : s.vm.typemap) {
	printer()(v.first);
	std::cout << "-->" << std::endl;
	print(v.second, "  ");
	std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "--- Funs ---" << std::endl << std::endl;

    for (const auto& v : s.vm.funs) {
	printer()(v.first);
	std::cout << "-->" << std::endl;
	print(v.second.type_from, "  ");
	std::cout << std::endl;
	print(v.second.type_to, "  ");
	std::cout << std::endl;

        for (const auto& tt : v.second.code) {
            print_element(tt);
            std::cout << std::endl;
        }
    }

}


crom::Val print_s(const crom::Val& v) {
    std::cout << crom::get<crom::String>(v) << std::endl;
    return (crom::Int)0;
}



int main(int argc, char** argv) {

  try {

      crom::Context stack;

      stack.vm.add_syscall("print", "String", "Int", print_s);


      if (argc > 3) {
          pegtl::trace_parse_string<crom::tunit>(true, 
                                                 pegtl::read_string(argv[1]),
                                                 stack);

      } else {
          pegtl::basic_parse_string<crom::tunit>(pegtl::read_string(argv[1]),
                                                 stack);
      }

      crom::check_types(stack.vm);
      print_context(stack);

      std::cout << "------------------------------" << std::endl;

      crom::run(stack.vm, false, crom::sym(argv[2]));
      

  } catch (std::runtime_error& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
