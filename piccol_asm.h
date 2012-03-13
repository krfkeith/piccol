#ifndef __NANOM_ASM_H
#define __NANOM_ASM_H

#include <cinttypes>

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

inline std::string int_to_string(Int i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%"PRIi64, i);
    return buff;
}

inline std::string uint_to_string(UInt i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%"PRIu64, i);
    return buff;
}

inline std::string real_to_string(Real i) {
    static char buff[1024];
    ::snprintf(buff, 1023, "%g", i);
    return buff;
}

}


struct VmAsm {

    VmCode code;

    VmAsm() : 
        nillabel(VmCode::toplevel_label()),
        vm(compiletime_code), 
        cmode(false),
        cmode_code(compiletime_code.codes[nillabel]),
        label(nillabel)
        {}

    label_t nillabel;

private:

    metalan::Symlist::list_t::const_iterator p_i;
    metalan::Symlist::list_t::const_iterator p_e;

    VmCode compiletime_code;
    Vm vm;

    bool cmode;
    VmCode::code_t& cmode_code;

    std::vector<Sym> shapestack;
    std::vector<label_t> labelstack;
    label_t label;

    void cmode_on() {
        cmode = true;
    }

    void cmode_off() {
        cmode = false;
        Opcode op;
        op.op = EXIT;
        cmode_code.push_back(op);

        vm_run(vm, nillabel);

        cmode_code.clear();
    }

    void push_type() {
        ++p_i;

        if (p_i == p_e)
            throw std::runtime_error("End of input in _push_type");

        shapestack.push_back(p_i->sym);
    }

    void pop_type() {
        if (shapestack.empty())
            throw std::runtime_error("Sanity error: _pop_type before _push_type");

        shapestack.pop_back();
    }

    void top_type() {
        if (shapestack.empty())
            throw std::runtime_error("Sanity error: _top_type before _push_type");

        Opcode op;
        op.op = PUSH;
        op.arg.uint = shapestack.back();

        code.codes[label].push_back(op);
    }

    void push_funlabel() {
        ++p_i;

        if (p_i == p_e)
            throw std::runtime_error("End of input in _push_funlabel");

        Sym name = p_i->sym;

        ++p_i;

        if (p_i == p_e)
            throw std::runtime_error("End of input in _push_funlabel");

        Sym from = p_i->sym;

        ++p_i;

        if (p_i == p_e)
            throw std::runtime_error("End of input in _push_funlabel");

        labelstack.emplace_back(name, from, p_i->sym);
        label = labelstack.back();

        if (code.codes.count(label) != 0) {
            throw std::runtime_error("Function defined twice: " + 
                                     symtab().get(label.name) + " " +
                                     symtab().get(label.fromshape) + "->" +
                                     symtab().get(label.toshape));
        }
    }

    void pop_funlabel() {
        if (labelstack.empty())
            throw std::runtime_error("Sanity error: _pop_funlabel before _push_funlabel");

        labelstack.pop_back();

        if (labelstack.empty()) {
            label = nillabel;
        } else {
            label = labelstack.back();
        }
    }

    void type_size() {
        if (shapestack.empty())
            throw std::runtime_error("Sanity error: _type_size before _push_type");

        Opcode op;
        op.op = PUSH;
        op.arg.uint = vm.shapes.get(shapestack.back()).size();

        code.codes[label].push_back(op);
    }

    void fieldname_deref() {
        if (shapestack.empty())
            throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

        ++p_i;

        if (p_i == p_e)
            throw std::runtime_error("End of input in _fieldname_deref");

        Sym fieldsym = p_i->sym;

        auto offrange = vm.shapes.get(shapestack.back()).get_index(fieldsym);

        if (offrange.first > offrange.second) {

            throw std::runtime_error("Unknown struct field: " + 
                                     symtab().get(shapestack.back()) + "." +
                                     symtab().get(fieldsym));
        }

        Opcode op;
        op.op = PUSH;
        op.arg.uint = offrange.first;

        code.codes[label].push_back(op);

        op.arg.uint = offrange.second;

        code.codes[label].push_back(op);
    }

    void fieldtype_check() {
        if (shapestack.empty())
            throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

        ++p_i;
        if (p_i == p_e)
            throw std::runtime_error("End of input in _fieldname_deref");

        Sym fieldsym = p_i->sym;

        ++p_i;
        if (p_i == p_e)
            throw std::runtime_error("End of input in _fieldname_deref");

        Sym typesym = p_i->sym;
        const std::string& typestr = symtab().get(typesym);

        const auto& typeinfo = vm.shapes.get(shapestack.back()).get_type(fieldsym);
        bool ok = false;
        std::string fieldtypename;

        switch (typeinfo.type) {
        case BOOL:
            if (typestr == "Bool") {
                ok = true;
            }
            fieldtypename = "<bool>";
            break;

        case INT:
        case UINT:
            if (typestr == "Int" || typestr == "UInt") {
                ok = true;
            }
            fieldtypename = "<integer>";
            break;

        case REAL:
            if (typestr == "Real") {
                ok = true;
            }
            fieldtypename = "<real>";
            break;

        case SYMBOL:
            if (typestr == "Sym") {
                ok = true;
            }
            fieldtypename = "<symbol>";
            break;

        case STRUCT:
            if (typesym == typeinfo.shape) {
                ok = true;
            }
            fieldtypename = symtab().get(typeinfo.shape);
            break;

        case NONE:
            break;
        }

        if (!ok) {
            throw std::runtime_error("Type checking failed: " +
                                     symtab().get(shapestack.back()) + "." +
                                     symtab().get(fieldsym) + " has type " + 
                                     fieldtypename + " but you assigned a " + typestr);
        }
    }

public:

    void parse(const metalan::Symlist& prog) {
    
        p_i = prog.syms.begin();
        p_e = prog.syms.end();
        label = nillabel;


        while (p_i != p_e) {

            const std::string& op_name = metalan::symtab().get(p_i->sym);

            if (op_name == "_cmode_on") {
                cmode_on();

                ++p_i;
                continue;

            } else if (op_name == "_cmode_off") {
                cmode_off();

                ++p_i;
                continue;

            } else if (op_name == "_push_type") {
                push_type();

                ++p_i;
                continue;

            } else if (op_name == "_pop_type") {
                pop_type();

                ++p_i;
                continue;

            } else if (op_name == "_top_type") {
                top_type();

                ++p_i;
                continue;

            } else if (op_name == "_push_funlabel") {
                push_funlabel();

                ++p_i;
                continue;

            } else if (op_name == "_pop_funlabel") {
                pop_funlabel();

                ++p_i;
                continue;

            } else if (op_name == "_type_size") {
                type_size();

                ++p_i;
                continue;

            } else if (op_name == "_fieldname_deref") {
                fieldname_deref();

                ++p_i;
                continue;

            } else if (op_name == "_fieldtype_check") {
                fieldtype_check();

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

                } else if (arg_type == "Int" || arg_type == "Bool") {
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


    std::string print() {

        metalan::Symlist tmp;

        for (const auto& i : code.codes) {

            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, "LABEL"));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.name));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.fromshape));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.toshape));

            for (const auto& j : i.second) {

                tmp.syms.push_back(metalan::Symcell(metalan::Symcell::QATOM, 
                                                    opcodename(j.op)));

                if (j.op == PUSH || j.op == PUSH_DUP) {
                    tmp.syms.push_back(metalan::Symcell(metalan::Symcell::QATOM, 
                                                        int_to_string(j.arg.uint)));
                }
            }
        }

        return tmp.print();
    }


    void parse(const std::string& pr) {

        metalan::Symlist prog;
        prog.parse(pr);

        parse(prog);
    }

};


}

#endif
