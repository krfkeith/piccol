#ifndef __PICCOL_ASM_H
#define __PICCOL_ASM_H

#include <cinttypes>

#include "metalan.h"

#include "nanom.h"


namespace piccol {

using namespace nanom;

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


struct PiccolAsm {

    PiccolAsm(Vm& _vm) : vm__(_vm)
        {}
    

private:

    Vm& vm__;


    struct asmcall_map_ {
        typedef std::unordered_map< std::pair<Sym,Sym>, 
                                    std::pair<Opcode,Sym> > map_t;
        
        map_t map;

        asmcall_map_() {

            add("[ Int Int ]",   "add",     ADD_INT,     "Int");
            add("[ Int Int ]",   "sub",     SUB_INT,     "Int");
            add("[ Int Int ]",   "mul",     MUL_INT,     "Int");
            add("[ Int Int ]",   "div",     DIV_INT,     "Int");
            add("Int",           "neg",     NEG_INT,     "Int");

            add("[ UInt UInt ]", "add",     ADD_UINT,    "UInt");
            add("[ UInt UInt ]", "sub",     SUB_UINT,    "UInt");
            add("[ UInt UInt ]", "mul",     MUL_UINT,    "UInt");
            add("[ UInt UInt ]", "div",     DIV_UINT,    "UInt");

            add("[ Real Real ]", "add",     ADD_REAL,    "Real");
            add("[ Real Real ]", "sub",     SUB_REAL,    "Real");
            add("[ Real Real ]", "mul",     MUL_REAL,    "Real");
            add("[ Real Real ]", "div",     DIV_REAL,    "Real");
            add("Real",          "neg",     NEG_REAL,    "Real");

            add("[ UInt UInt ]", "band",    BAND,        "UInt");
            add("[ UInt UInt ]", "bor",     BOR,         "UInt");
            add("[ UInt UInt ]", "bnot",    BNOT,        "UInt");
            add("[ UInt UInt ]", "bxor",    BXOR,        "UInt");
            add("[ UInt UInt ]", "bshl",    BSHL,        "UInt");
            add("[ UInt UInt ]", "bshr",    BSHR,        "UInt");

            add("[ Int Int ]",   "eq",      EQ_INT,      "Bool");
            add("[ Int Int ]",   "lt",      LT_INT,      "Bool");
            add("[ Int Int ]",   "lte",     LTE_INT,     "Bool");
            add("[ Int Int ]",   "gt",      GT_INT,      "Bool");
            add("[ Int Int ]",   "gte",     GTE_INT,     "Bool");

            add("[ UInt UInt ]", "eq",      EQ_UINT,      "Bool");
            add("[ UInt UInt ]", "lt",      LT_UINT,      "Bool");
            add("[ UInt UInt ]", "lte",     LTE_UINT,     "Bool");
            add("[ UInt UInt ]", "gt",      GT_UINT,      "Bool");
            add("[ UInt UInt ]", "gte",     GTE_UINT,     "Bool");

            add("[ Real Real ]", "eq",      EQ_REAL,      "Bool");
            add("[ Real Real ]", "lt",      LT_REAL,      "Bool");
            add("[ Real Real ]", "lte",     LTE_REAL,     "Bool");
            add("[ Real Real ]", "gt",      GT_REAL,      "Bool");
            add("[ Real Real ]", "gte",     GTE_REAL,     "Bool");

            add("[ Bool Bool ]", "and",     BAND,         "Bool");
            add("[ Bool Bool ]", "or",      BOR,          "Bool");
            add("Bool",          "not",     BOOL_NOT,     "Bool");

            add("Int",           "to_real", INT_TO_REAL,  "Real");
            add("Real",          "to_int",  REAL_TO_INT,  "Int");

            add("Int",           "to_sym",  INT_TO_CHAR,  "Sym");
            add("UInt",          "to_sym",  UINT_TO_CHAR, "Sym");
            
            add("Int",           "to_uint", NOOP,         "UInt");
            add("Int",           "to_bool", NOOP,         "Bool");
            add("UInt",          "to_int",  NOOP,         "Int");
            add("UInt",          "to_bool", NOOP,         "Bool");
            add("Bool",          "to_int",  NOOP,         "Int");
            add("Bool",          "to_uint", NOOP,         "UInt");

            add("Bool",          "if",      IF,           "Void");
        }

        void add(const std::string& typefrom, const std::string& name,
                 op_t op, const std::string& typeto) {

            map[std::make_pair(symtab().get(typefrom), symtab().get(name))] = 
                std::make_pair(Opcode(op), symtab().get(typeto));
        }
    };

    struct compile_ctx {

        compile_ctx(const label_t& nill, Shapes& oldshapes, VmCode& runtime_code) : 
            nillabel(nill),
            compiletime_vm(compiletime_code, oldshapes),
            cmode(false),
            cmode_code(compiletime_code.codes[nillabel]),
            code(runtime_code),
            label(nill),
            curbranch(0)
            {
                compiletime_vm.shapes = oldshapes;
            }

        label_t nillabel;
        VmCode compiletime_code;
        Vm compiletime_vm;

        metalan::Symlist::list_t::const_iterator p_i;
        metalan::Symlist::list_t::const_iterator p_e;

        bool cmode;
        VmCode::code_t& cmode_code;
        VmCode& code;

        std::vector<Sym> shapestack;
        std::vector<label_t> labelstack;
        label_t label;
        size_t curbranch;


        void cmode_on() {
            cmode = true;
        }

        void cmode_off() {
            cmode = false;
            Opcode op;
            op.op = EXIT;
            cmode_code.push_back(op);

            vm_run(compiletime_vm, nillabel);

            cmode_code.clear();
        }

        void push_type() {
            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("End of input in _push_type");

            if (!compiletime_vm.shapes.has_shape(p_i->sym)) 
                throw std::runtime_error("Undefined shape: " + symtab().get(p_i->sym));

            shapestack.push_back(p_i->sym);
        }

        void pop_type() {
            if (shapestack.empty())
                throw std::runtime_error("Sanity error: _pop_type before _push_type");

            shapestack.pop_back();
        }

        void drop_types() {
            shapestack.clear();
        }

        void top_type() {
            if (shapestack.empty())
                throw std::runtime_error("Sanity error: _top_type before _push_type");

            Opcode op;
            op.op = PUSH;
            op.arg.uint = shapestack.back();

            code.codes[label].push_back(op);
        }

        void mark_tuple() {

            shapestack.push_back(symtab().get(""));
        }

        void make_tupletype() {
        
            std::vector<Sym> tuptypes;

            while (1) {
                if (shapestack.empty()) 
                    throw std::runtime_error("Sanity error: _make_tupletype before _mark_tuple");

                Sym s = shapestack.back();
                shapestack.pop_back();

                if (s == symtab().get(""))
                    break;

                tuptypes.push_back(s);
            }

            std::string hacktype = "[";
            for (std::vector<Sym>::reverse_iterator i = tuptypes.rbegin(); i != tuptypes.rend(); ++i) {
                hacktype += " ";
            
                const std::string& tn = symtab().get(*i);
                hacktype += tn;
            }
            hacktype += " ]";

            Sym tupshs = symtab().get(hacktype);

            if (!compiletime_vm.shapes.has_shape(tupshs)) 
                throw std::runtime_error("Undefined shape: " + symtab().get(tupshs));

            shapestack.push_back(tupshs);
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
            curbranch = 0;

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

            const label_t& fname = labelstack.back();

            if (shapestack.empty()) {

                if (fname.toshape != symtab().get("Void")) {
                    throw std::runtime_error("Sanity error: _pop_funlabel before _push_type");
                }

            } else {

                if (fname.toshape != shapestack.back()) {

                    throw std::runtime_error("Wrong return type: " + 
                                             symtab().get(fname.name) + " " +
                                             symtab().get(fname.fromshape) + "->" +
                                             symtab().get(fname.toshape) + " returns " + 
                                             symtab().get(shapestack.back()));
                }
            }

            labelstack.pop_back();

            if (labelstack.empty()) {
                label = nillabel;
            } else {
                label = labelstack.back();
            }
        }

        void push_branch() {

            if (labelstack.empty())
                throw std::runtime_error("Sanity error: _push_branch before _push_funlabel");

            curbranch++;

            label_t l(labelstack.back());
            l.name = symtab().get(symtab().get(l.name) + "$" + uint_to_string(curbranch));

            code.codes[label].push_back(Opcode(PUSH, (UInt)l.name));

            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("End of input in _push_branch");

            // HACK
            code.codes[label].push_back(Opcode(opcodecode(symtab().get(p_i->sym))));

            labelstack.push_back(l);
            label = labelstack.back();
        }

        void push_lambda() {

            if (labelstack.empty())
                throw std::runtime_error("Sanity error: _push_branch before _push_funlabel");

            if (shapestack.empty())
                throw std::runtime_error("Sanity error: _push_lambda before _push_type");

            curbranch++;

            label_t l(labelstack.back());
            l.name = symtab().get(symtab().get(l.name) + "$" + uint_to_string(curbranch));
            l.fromshape = shapestack.back();

            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("End of input in _push_lambda");

            l.toshape = p_i->sym;

            code.codes[label].push_back(Opcode(PUSH, (UInt)l.name));
            code.codes[label].push_back(Opcode(PUSH, (UInt)l.fromshape));
            code.codes[label].push_back(Opcode(PUSH, (UInt)l.toshape));

            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("End of input in _push_lambda");

            // HACK
            code.codes[label].push_back(Opcode(opcodecode(symtab().get(p_i->sym))));

            labelstack.push_back(l);
            label = labelstack.back();
        }

        void type_size() {
            if (shapestack.empty())
                throw std::runtime_error("Sanity error: _type_size before _push_type");

            Opcode op;
            op.op = PUSH;
            op.arg.uint = compiletime_vm.shapes.get(shapestack.back()).size();

            code.codes[label].push_back(op);
        }

        void fieldname_deref() {
            if (shapestack.empty())
                throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("End of input in _fieldname_deref");

            Sym fieldsym = p_i->sym;

            if (shapestack.size() < 2) 
                throw std::runtime_error("Invalid _fieldname_deref");

            Sym structshape = shapestack[shapestack.size()-2];

            auto offrange = compiletime_vm.shapes.get(structshape).get_index(fieldsym);

            if (offrange.first > offrange.second) {

                throw std::runtime_error("Unknown struct field: " + 
                                         symtab().get(structshape) + "." +
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

            if (shapestack.size() < 2) 
                throw std::runtime_error("Invalid _fieldtype_check");

            Sym structshape = shapestack[shapestack.size()-2];
            Sym valtype = shapestack.back();
            const std::string& typestr = symtab().get(valtype);

            const auto& typeinfo = compiletime_vm.shapes.get(structshape).get_type(fieldsym);
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
                if (valtype == typeinfo.shape) {
                    ok = true;
                }
                fieldtypename = symtab().get(typeinfo.shape);
                break;

            case NONE: 
                throw std::runtime_error("No such field: " + 
                                         symtab().get(structshape) + "." +
                                         symtab().get(fieldsym));
                break;
            }

            if (!ok) {
                throw std::runtime_error("Type checking failed: " +
                                         symtab().get(structshape) + "." +
                                         symtab().get(fieldsym) + " has type " + 
                                         fieldtypename + " but you assigned a " + typestr);
            }
        }

        
        void get_fields() {
        
            if (labelstack.empty())
                throw std::runtime_error("Sanity error: _get_fields before _push_funlabel");

            Sym framehead = labelstack.back().fromshape;
            const Shape& sh = compiletime_vm.shapes.get(framehead);

            ++p_i;
            if (p_i == p_e)
                throw std::runtime_error("End of input in _get_fields");

            Sym fieldsym = p_i->sym;

            const Shape::typeinfo& fieldt = sh.get_type(fieldsym);
            Sym newshape;

            switch (fieldt.type) {
            case BOOL:   newshape = symtab().get("Bool"); break;
            case INT:    newshape = symtab().get("Int"); break;
            case UINT:   newshape = symtab().get("UInt"); break;
            case REAL:   newshape = symtab().get("Real"); break;
            case SYMBOL: newshape = symtab().get("Sym"); break;
            case STRUCT: newshape = fieldt.shape; break;
            case NONE:
                throw std::runtime_error("No such field: " + 
                                         symtab().get(framehead) + "." +
                                         symtab().get(fieldsym));
            }

            shapestack.push_back(newshape);

            code.codes[label].push_back(Opcode(PUSH, (UInt)fieldt.ix_from));
            code.codes[label].push_back(Opcode(PUSH, (UInt)fieldt.ix_to));
        }


        void call_or_syscall() {

            if (shapestack.empty()) 
                throw std::runtime_error("_call_or_syscall before _push_type");

            ++p_i;
            if (p_i == p_e)
                throw std::runtime_error("End of input in _call_or_syscall");

            Sym name = p_i->sym;

            ++p_i;
            if (p_i == p_e)
                throw std::runtime_error("End of input in _call_or_syscall");

            Sym toshape = p_i->sym;
            Sym fromshape = shapestack.back();
        
            label_t l(name, fromshape, toshape);

            code.codes[label].push_back(Opcode(PUSH, (UInt)name));
            code.codes[label].push_back(Opcode(PUSH, (UInt)fromshape));
            code.codes[label].push_back(Opcode(PUSH, (UInt)toshape));

            if (code.codes.find(l) != code.codes.end()) {
                code.codes[label].push_back(Opcode(CALL));

            } else if (code.callbacks.find(l) != code.callbacks.end()) {
                code.codes[label].push_back(Opcode(SYSCALL));

            } else {
            
                throw std::runtime_error("Undefined function called: " + 
                                         symtab().get(name) + " " + 
                                         symtab().get(fromshape) + "->" + 
                                         symtab().get(toshape));
            }

            shapestack.pop_back();
            shapestack.push_back(toshape);
        }


        bool asmcall(Sym shape, Sym call) {
            static asmcall_map_ amap;

            auto i = amap.map.find(std::make_pair(shape, call));

            if (i == amap.map.end())
                return false;

            // HACK
            // Too lazy yet to implement a proper multi-opcode feature.
            if (i->second.first.op == IF) {

                code.codes[label].push_back(Opcode(IF, (Int)2));
                code.codes[label].push_back(Opcode(FAIL));
                
            } else {

                code.codes[label].push_back(i->second.first);
            }

            shapestack.push_back(i->second.second);

            return true;
        }

        void asmcall() {

            Sym shape = symtab().get("Void");

            if (!shapestack.empty()) {
                shape = shapestack.back();
                shapestack.pop_back();
            }

            ++p_i;
            if (p_i == p_e)
                throw std::runtime_error("End of input in _asmcall");

            Sym s = p_i->sym;

            if (!asmcall(shape, s)) {

                throw std::runtime_error("Unknown syscall: " + symtab().get(shape) + " " + symtab().get(s));
            }
        }


        void parse(const metalan::Symlist& prog) {
  
            p_i = prog.syms.begin();
            p_e = prog.syms.end();
            label = nillabel;


            while (p_i != p_e) {

                const std::string& op_name = metalan::symtab().get(p_i->sym);

                /*
                  std::cout << "!" << op_name << std::endl;
                  for (const auto& s : shapestack) 
                      std::cout << " " << symtab().get(s);
                  std::cout << std::endl;
                  for (const auto& s : labelstack) 
                      std::cout << symtab().get(s.name) << " " << symtab().get(s.fromshape)
                                << " " << symtab().get(s.toshape) << std::endl;
                  std::cout << "--- ---" << std::endl;
                */

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

                } else if (op_name == "_drop_types") {
                    drop_types();

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

                } else if (op_name == "_push_branch") {
                    push_branch();

                    ++p_i;
                    continue;

                } else if (op_name == "_push_lambda") {
                    push_lambda();

                    ++p_i;
                    continue;

                } else if (op_name == "_type_size") {
                    type_size();

                    ++p_i;
                    continue;

                } else if (op_name == "_mark_tuple") {
                    mark_tuple();

                    ++p_i;
                    continue;

                } else if (op_name == "_make_tupletype") {
                    make_tupletype();

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

                } else if (op_name == "_get_fields") {
                    get_fields();
                
                    ++p_i;
                    continue;

                } else if (op_name == "_call_or_syscall") {
                    call_or_syscall();

                    ++p_i;
                    continue;

                } else if (op_name == "_asmcall") {
                    asmcall();

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
            
                if (op.op == PUSH) {

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

                } else if (op.op == IF || op.op == IF_NOT || op.op == IF_FAIL || op.op == IF_NOT_FAIL) {

                    if (p_i == p_e) {
                        throw std::runtime_error("End of input while looking for opcode argument");
                    }

                    op.arg = string_to_int(metalan::symtab().get(p_i->sym));
                    ++p_i;
                }
            
                c.push_back(op);

                if (cmode) {
                    cmode_code.push_back(op);
                }
            }
        }
    };


        
public:

    void parse(const metalan::Symlist& prog) {

        compile_ctx ctx(VmCode::toplevel_label(), vm__.shapes, vm__.code);
        ctx.parse(prog);
    }

    std::string print() {

        metalan::Symlist tmp;

        for (const auto& i : vm__.code.codes) {

            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, "LABEL"));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.name));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.fromshape));
            tmp.syms.push_back(metalan::Symcell(metalan::Symcell::VAR, i.first.toshape));

            for (const auto& j : i.second) {

                tmp.syms.push_back(metalan::Symcell(metalan::Symcell::QATOM, 
                                                    opcodename(j.op)));

                if (j.op == PUSH || j.op == IF || j.op == IF_NOT || j.op == IF_FAIL || j.op == IF_NOT_FAIL) {
                    tmp.syms.push_back(metalan::Symcell(metalan::Symcell::QATOM, 
                                                        j.arg.uint)); //int_to_string(j.arg.uint)));
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
