#ifndef __NANOM_ASM_H
#define __NANOM_ASM_H

#include "metalan.h"

#include "nanom.h"


namespace nanom {

namespace {

inline Int string_to_int(const std::string& s) {
    return (Int)::strtoll(s.c_str(), NULL, 10);
}

inline UInt string_to_uint(const std::string& s) {
    return (UInt)::strtoul(s.c_str(), NULL, 10);
}

inline Real string_to_real(const std::string& s) {
    return (Real)::strtod(s.c_str(), NULL);
}

}


struct VmAsm {

    VmCode code;

    void parse(const metalan::Symlist& prog) {
    
        Sym nillabel = metalan::symtab().get("");
        Sym label = nillabel;

        auto p_i = prog.syms.begin();
        auto p_e = prog.syms.end();

        VmCode compiletime_code;

        bool cmode = false;
        auto& cmode_code= compiletime_code.codes[nillabel];

        std::vector<Sym> shapestack;

        while (p_i != p_e) {

            const std::string& op_name = metalan::symtab().get(p_i->sym);

            if (op_name == "_cmode_on") {
                cmode = true;

                ++p_i;
                continue;

            } else if (op_name == "_cmode_off") {
                cmode = false;
                Opcode op;
                op.op = EXIT;
                cmode_code.push_back(op);

                Vm vm(compiletime_code);
                vm_run(vm, nillabel);

                cmode_code.clear();

                ++p_i;
                continue;

            } else if (op_name == "_push_type") {
                ++p_i;

                if (p_i == p_e)
                    throw std::runtime_error("End of input in _push_type");

                shapestack.push_back(p_i->sym);
                
                ++p_i;
                continue;

            } else if (op_name == "_pop_type") {

                if (shapestack.empty())
                    throw std::runtime_error("Sanity error: _pop_type before _push_type");

                shapestack.pop_back();

                ++p_i;
                continue;

            } else if (op_name == "_type_size") {

                if (shapestack.empty())
                    throw std::runtime_error("Sanity error: _type_size before _push_type");

                Opcode op;
                op.op = PUSH;
                op.arg.uint = shapes().get(shapestack.back()).size();

                code.codes[label].push_back(op);

                ++p_i;
                continue;

            } else if (op_name == "_fieldname_deref") {

                if (shapestack.empty())
                    throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

                ++p_i;

                if (p_i == p_e)
                    throw std::runtime_error("End of input in _fieldname_deref");

                Sym fieldsym = p_i->sym;

                auto offrange = shapes().get(shapestack.back()).get_index(fieldsym);

                Opcode op;
                op.op = PUSH;
                op.arg.uint = offrange.first;

                code.codes[label].push_back(op);

                op.arg.uint = offrange.second;

                code.codes[label].push_back(op);

                ++p_i;
                continue;
            }


            auto& c = code.codes[label];

            Opcode op;

            op.op = opcodecode(op_name);

            if (op.op == 0) {
                throw std::runtime_error("Unknown opcode: " + op_name);
            }

            ++p_i;
            
            if (op.op == PUSH ||
                op.op == PUSH_DUP) {

                if (p_i == p_e) {
                    throw std::runtime_error("End of input while looking for opcode argument");
                }

                const std::string& arg_type = metalan::symtab().get(p_i->sym);

                ++p_i;

                if (p_i == p_e) {
                    throw std::runtime_error("End of input while looking for opcode argument");
                }

                if (arg_type == "Sym") {
                    op.arg = p_i->sym;

                } else if (arg_type == "Int") {
                    op.arg = string_to_int(metalan::symtab().get(p_i->sym));

                } else if (arg_type == "UInt") {
                    op.arg = string_to_uint(metalan::symtab().get(p_i->sym));
                
                } else if (arg_type == "Real") {
                    op.arg = string_to_real(metalan::symtab().get(p_i->sym));

                } else {
                    throw std::runtime_error("Unknown opcode argument type: " + arg_type);
                }

                ++p_i;
            }
            
            c.push_back(op);

            if (cmode) {
                cmode_code.push_back(op);
            }

        }
    }



    void parse(const std::string& pr) {

        metalan::Symlist prog;
        prog.parse(pr);

        parse(prog);
    }

};


}

#endif
