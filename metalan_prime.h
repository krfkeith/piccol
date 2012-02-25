#ifndef __METALAN_PRIME_H
#define __METALAN_PRIME_H

#include <map>

#include "metalan.h"
#include "nanom.h"
#include "nanom_stringlib.h"


namespace metalan {

struct fmter {
    std::string& str;
    fmter(std::string& s) : str(s) {}

    fmter& operator<<(const char* s) {
        str += s;
        return *this;
    }

    fmter& operator<<(const std::string& s) {
        str += s;
        return *this;
    }

    fmter& operator<<(nanom::Int v) {
        str += nanom::int_to_string(v);
        return *this;
    }

    fmter& operator<<(nanom::UInt v) {
        str += nanom::uint_to_string(v);
        return *this;
    }

    fmter& operator<<(nanom::Real v) {
        str += nanom::real_to_string(v);
        return *this;
    }

    fmter& operator<<(int v) {
        str += nanom::int_to_string((nanom::Int)v);
        return *this;
    }

    fmter& operator<<(unsigned int v) {
        str += nanom::uint_to_string((nanom::Int)v);
        return *this;
    }
};


struct NanomAsmProcessor {

    static const size_t symbol_base = 0xFF0000;

    nanom::Vm vm;
    nanom::Assembler vm_as;

    Parser parser;

    NanomAsmProcessor() : vm_as(vm) 
        {
            nanom::register_stringlib(vm, 0);

            vm_as.register_const("port", (nanom::UInt)0xFF0000);
            vm_as.register_const("in", (nanom::UInt)0);
            vm_as.register_const("out", (nanom::UInt)1);

            vm_as.register_const("str_append_char", (nanom::UInt)1);
            vm_as.register_const("int_to_str", (nanom::UInt)2);
            vm_as.register_const("uint_to_str", (nanom::UInt)3);
            vm_as.register_const("real_to_str", (nanom::UInt)4);
            vm_as.register_const("str_to_int", (nanom::UInt)5);
            vm_as.register_const("str_to_uint", (nanom::UInt)6);
            vm_as.register_const("str_to_real", (nanom::UInt)7);
            vm_as.register_const("str_append", (nanom::UInt)8);
            vm_as.register_const("str_free", (nanom::UInt)9);
        }

    void parse(const std::string& code, const std::string& inp) {

        Outlist out;
        std::string unprocessed;
        bool ok = parser.parse(code, inp, out, unprocessed);

        if (!ok) {
            throw std::runtime_error("Parse failed. Unconsumed input: " + unprocessed);
        }

        std::string asmprog;

        std::map<size_t,std::string> result;

        fmter f(asmprog);

        f << "PUSH($port)\n"
          << "SIZE_HEAP(2)\n";

        for (const auto& n : out) {

            size_t inputid = symbol_base + result.size() * 2;
            size_t outputid = inputid + 1;
            std::string& respart = result[outputid];

            if (n.type == Outnode::CODE) {

                f << "\n.symbol '";

                for (unsigned char c : n.capture) {
                    if (c == '\'') asmprog += "\\'";
                    else asmprog += c;
                }

                f << "' " << inputid << "\n"
                  << "PUSH(" << inputid << ")\n"
                  << "PUSH($port)\n"
                  << "TO_HEAP($in)\n"
                  << "PUSH(" << outputid << ")\n"
                  << "PUSH($port)\n"
                  << "TO_HEAP($out)\n";

                asmprog += n.str;

            } else {
                respart = n.str;
            }
        }

        asmprog += "\nEXIT\n";
        

        std::cout << asmprog << std::endl << std::endl;
        std::cout << "-----------------------------" << std::endl;

        vm_as.assemble(asmprog);
        vm_as.vm_run("main");

        for (auto& c : result) {

            auto i = vm.symtab.find(c.first);

            if (i != vm.symtab.end()) {
                std::cout << i->second;
            } else {
                std::cout << c.second;
            }
        }

        std::cout << std::endl;
    }
};

}

#endif
