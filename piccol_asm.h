#ifndef __PICCOL_ASM_H
#define __PICCOL_ASM_H

#include <cinttypes>
#include <map>

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
    char buff[256];
    ::snprintf(buff, 255, "%" PRIi64, i);
    return buff;
}

inline std::string uint_to_string(UInt i) {
    char buff[256];
    ::snprintf(buff, 255, "%" PRIu64, i);
    return buff;
}

inline std::string real_to_string(Real i) {
    char buff[256];
    ::snprintf(buff, 255, "%g", i);
    return buff;
}

}


struct PiccolAsm {

    PiccolAsm(Vm& _vm) : vm__(_vm)
        {}
    

private:

    Vm& vm__;


    struct asmcall_map_ {
        typedef std::map< std::pair<Sym,Sym>, 
                          std::pair<Opcode,Sym> > map_t;
        
        map_t map;

        asmcall_map_() {

            add("[ Int Int ]",   "add",     ADD_INT,     "Int");
            add("[ Int Int ]",   "sub",     SUB_INT,     "Int");
            add("[ Int Int ]",   "mul",     MUL_INT,     "Int");
            add("[ Int Int ]",   "div",     DIV_INT,     "Int");
            add("[ Int Int ]",   "mod",     MOD_INT,     "Int");
            add("Int",           "neg",     NEG_INT,     "Int");

            add("[ UInt UInt ]", "add",     ADD_UINT,    "UInt");
            add("[ UInt UInt ]", "sub",     SUB_UINT,    "UInt");
            add("[ UInt UInt ]", "mul",     MUL_UINT,    "UInt");
            add("[ UInt UInt ]", "div",     DIV_UINT,    "UInt");
            add("[ UInt UInt ]", "mod",     MOD_UINT,    "UInt");

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

            add("[ Sym Sym ]", "eq", EQ_UINT, "Bool");

            add("[ Bool Bool ]", "and",     BAND,         "Bool");
            add("[ Bool Bool ]", "or",      BOR,          "Bool");
            add("Bool",          "not",     BOOL_NOT,     "Bool");

            add("Int",           "to_real", INT_TO_REAL,  "Real");
            add("UInt",          "to_real", UINT_TO_REAL, "Real");
            add("Real",          "to_int",  REAL_TO_INT,  "Int");
            add("Real",          "to_uint", REAL_TO_UINT, "UInt");

            add("Int",           "to_sym",  INT_TO_CHAR,  "Sym");
            add("UInt",          "to_sym",  UINT_TO_CHAR, "Sym");
            
            add("Int",           "to_uint", NOOP,         "UInt");
            add("Int",           "to_bool", NOOP,         "Bool");
            add("UInt",          "to_int",  NOOP,         "Int");
            add("UInt",          "to_bool", NOOP,         "Bool");
            add("Bool",          "to_int",  NOOP,         "Int");
            add("Bool",          "to_uint", NOOP,         "UInt");

            // NOTE!
            // These ridiculous constructs are only needed to make
            // parse trees (i.e. automatic generation of piccol code)
            // easier to generate!
            add("[ Int ]",  "noop", NOOP, "Int");
            add("[ UInt ]", "noop", NOOP, "UInt");
            add("[ Bool ]", "noop", NOOP, "Bool");
            add("[ Real ]", "noop", NOOP, "Real");
            add("[ Sym ]",  "noop", NOOP, "Sym");

            add("Bool",          "if",      IF,           "Void");
            add("Void",          "fail",    FAIL,         "");
        }

        void add(const std::string& typefrom, const std::string& name,
                 op_t op, const std::string& typeto) {

            Sym s_typeto;

            if (typeto == "") {
                s_typeto = 0;
            } else {
                s_typeto = symtab().get(typeto);
            }

            map[std::make_pair(symtab().get(typefrom), symtab().get(name))] = 
                std::make_pair(Opcode(op), s_typeto);
        }
    };

    struct compile_ctx {

        compile_ctx(const label_t& nill, Shapes& oldshapes, VmCode& runtime_code) : 
            nillabel(nill),
            compiletime_vm(compiletime_code, oldshapes),
            cmode(false),
            cmode_code(compiletime_code.codes[nillabel]),
            code(runtime_code)
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

        struct funsig {
            label_t label;
            std::vector<Sym> shapestack;
            size_t curbranch;

            funsig() {}
            funsig(Sym a, Sym b, Sym c) : label(a, b, c), curbranch(0) {}
        };

        std::vector<funsig> labelstack;


        void cmode_on() {
            cmode = true;
        }

        void cmode_off() {
            cmode = false;
            Opcode op;
            op.op = EXIT;
            cmode_code.push_back(op);

            compiletime_vm.frame.emplace_back(nillabel, 0, 0, 0);

            vm_run(compiletime_vm, nillabel);

            cmode_code.clear();
        }

        const label_t& label(bool safe=true) {

            if (labelstack.empty()) {
                if (safe && !cmode) {
                    throw std::runtime_error("Sanity error: label() before _push_funlabel");
                } else {
                    return nillabel;
                }
            }

            return labelstack.back().label;
        }

        std::vector<Sym>& shapestack() {
            if (labelstack.empty()) {
                throw std::runtime_error("Sanity error: shapestack() before _push_funlabel");
            }

            return labelstack.back().shapestack;
        }

        Sym next() {
            ++p_i;

            if (p_i == p_e)
                throw std::runtime_error("Sanity error: end of input in system opcode");

            return p_i->sym;
        }


        void push_type() {

            Sym sym = next();

            if (!compiletime_vm.shapes.has_shape(sym)) 
                throw std::runtime_error("Undefined shape: " + symtab().get(sym));

            shapestack().push_back(sym);
        }

        void pop_type() {

            auto& ss = shapestack();

            if (ss.empty())
                throw std::runtime_error("Sanity error: _pop_type before _push_type");

            ss.pop_back();
        }

        void top_type() {

            auto& ss = shapestack();

            if (ss.empty())
                throw std::runtime_error("Sanity error: _top_type before _push_type");

            Opcode op;
            op.op = PUSH;
            op.arg.uint = ss.back();

            code.codes[label()].push_back(op);
        }

        void mark_tuple() {

            shapestack().push_back(symtab().get(""));
        }

        void make_tupletype() {

            auto& ss = shapestack();
        
            std::vector<Sym> tuptypes;

            while (1) {
                if (ss.empty()) 
                    throw std::runtime_error("Sanity error: _make_tupletype before _mark_tuple");

                Sym s = ss.back();
                ss.pop_back();

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

            ss.push_back(tupshs);
        }

        void push_funlabel() {

            Sym name = next();
            Sym from = next();
            Sym to = next();

            labelstack.emplace_back(name, from, to);

            auto& ss = shapestack();

            if (code.codes.count(label()) != 0) {
                throw std::runtime_error("Function defined twice: " + label().print());
            }

            ss.push_back(label().fromshape);
            ss.push_back(symtab().get("Void"));
        }

        void pop_funlabel() {

            const auto& ss = shapestack();

            const label_t& fname = label();

            if (ss.size() < 2) {
                throw std::runtime_error("Function does not return a value: " + fname.print());
            }

            static Sym voidsym = symtab().get("Void");

            auto i = ss.begin();
            auto e = ss.end();
            ++i;

            size_t ntypes = 0;
            while (i != e) {
                if (*i != voidsym) {
                    ntypes++;
                }
                ++i;
            }

            if (ntypes > 1) {
                throw std::runtime_error("Function returns more than one value: " + fname.print());
            }

            Sym tmp = ss.back();

            // The 'nil' type denotes a function that fails.
            // If a function fails, then it doesn't really have a return type.
            if (tmp == 0) {
                tmp = fname.toshape;

            } else {

                if (fname.toshape != tmp) {

                    throw std::runtime_error("Wrong return type: " + fname.print() + " returns " + 
                                             symtab().get(ss.back()));
                }
            }

            labelstack.pop_back();

            if (!labelstack.empty()) {
                shapestack().push_back(tmp);
            }
        }

        void push_branch() {

            auto& ss = shapestack();

            size_t& curbranch = labelstack.back().curbranch;
            curbranch++;

            label_t l = label();

            if (ss.size() < 2)
                throw std::runtime_error("Sanity error: _push_branch before _push_type");

            // Forget the type of the result of the previous branch.
            ss.pop_back();

            Sym tmp = ss.back();

            l.name = symtab().get(symtab().get(l.name) + "$" + uint_to_string(curbranch));

            code.codes[label()].push_back(Opcode(PUSH, (UInt)l.name));

            Sym nextopcode = next();

            // HACK
            code.codes[label()].push_back(Opcode(opcodecode(symtab().get(nextopcode))));

            labelstack.emplace_back(l.name, l.fromshape, l.toshape);

            shapestack().push_back(tmp);
        }

        void push_lambda() {

            auto& ss = shapestack();

            size_t& curbranch = labelstack.back().curbranch;            
            curbranch++;

            label_t l = label();

            if (ss.size() < 2)
                throw std::runtime_error("Sanity error: _push_branch before _push_type");

            l.name = symtab().get(symtab().get(l.name) + "$" + uint_to_string(curbranch));
            l.fromshape = ss.back();

            //
            ss.pop_back();

            l.toshape = next();

            code.codes[label()].push_back(Opcode(PUSH, (UInt)l.name));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)l.fromshape));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)l.toshape));

            Sym nextopcode = next();

            // HACK
            code.codes[label()].push_back(Opcode(opcodecode(symtab().get(nextopcode))));

            labelstack.emplace_back(l.name, l.fromshape, l.toshape);

            shapestack().push_back(l.fromshape);
            shapestack().push_back(symtab().get("Void"));
        }

        void type_size() {

            const auto& ss = shapestack();

            if (ss.empty())
                throw std::runtime_error("Sanity error: _type_size before _push_type");

            Opcode op;
            op.op = PUSH;
            op.arg.uint = compiletime_vm.shapes.get(ss.back()).size();

            code.codes[label()].push_back(op);
        }

        void fieldname_deref() {

            const auto& ss = shapestack();

            if (ss.empty())
                throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

            Sym fieldsym = next();

            if (ss.size() < 2) 
                throw std::runtime_error("Invalid _fieldname_deref");

            Sym structshape = ss[ss.size()-2];

            auto offrange = compiletime_vm.shapes.get(structshape).get_index(fieldsym);

            if (offrange.first > offrange.second) {

                throw std::runtime_error("Unknown struct field: " + 
                                         symtab().get(structshape) + "." +
                                         symtab().get(fieldsym));
            }

            Opcode op;
            op.op = PUSH;
            op.arg.uint = offrange.first;

            code.codes[label()].push_back(op);

            op.arg.uint = offrange.second;

            code.codes[label()].push_back(op);
        }

        void fieldtype_check() {

            const auto& ss = shapestack();

            if (ss.empty())
                throw std::runtime_error("Sanity error: _fieldname_deref before _push_type");

            Sym fieldsym = next();

            if (ss.size() < 2) 
                throw std::runtime_error("Invalid _fieldtype_check");

            Sym structshape = ss[ss.size()-2];
            Sym valtype = ss.back();
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
        
            Sym framehead = label().fromshape;
            const Shape& sh = compiletime_vm.shapes.get(framehead);

            Sym fieldsym = next(); 

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

            shapestack().push_back(newshape);

            code.codes[label()].push_back(Opcode(PUSH, (UInt)fieldt.ix_from));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)fieldt.ix_to));
        }

        void get_all_fields() {
        
            Sym framehead = label().fromshape;
            const Shape& sh = compiletime_vm.shapes.get(framehead);

            shapestack().push_back(framehead);

            code.codes[label()].push_back(Opcode(PUSH, (UInt)0));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)sh.size()));
        }


        void call_or_syscall(bool tailcall) {

            auto& ss = shapestack();

            if (ss.empty()) 
                throw std::runtime_error("_call_or_syscall before _push_type");

            Sym name = next();
            Sym toshape = next();
            Sym fromshape = ss.back();
        
            label_t l(name, fromshape, toshape);

            code.codes[label()].push_back(Opcode(PUSH, (UInt)name));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)fromshape));
            code.codes[label()].push_back(Opcode(PUSH, (UInt)toshape));

            if (code.codes.find(l) != code.codes.end()) {
                code.codes[label()].push_back(Opcode(tailcall ? TAILCALL : CALL));

            } else if (code.callbacks.find(l) != code.callbacks.end()) {
                code.codes[label()].push_back(Opcode(SYSCALL));

            } else {
            
                throw std::runtime_error("Undefined function called: " + l.print());
            }

            ss.pop_back();

            ss.push_back(toshape);
        }


        bool asmcall(Sym shape, Sym call) {
            static const asmcall_map_ amap;

            auto i = amap.map.find(std::make_pair(shape, call));

            if (i == amap.map.end())
                return false;

            // HACK
            // Too lazy yet to implement a proper multi-opcode feature.
            if (i->second.first.op == IF) {

                code.codes[label()].push_back(Opcode(IF, (Int)2));
                code.codes[label()].push_back(Opcode(FAIL));

            } else {

                if (i->second.first.op != NOOP) {
                    code.codes[label()].push_back(i->second.first);
                }
            }

            shapestack().push_back(i->second.second);

            return true;
        }

        void asmcall() {

            Sym shape = shapestack().back();
            shapestack().pop_back();

            Sym s = next();

            if (!asmcall(shape, s)) {

                throw std::runtime_error("Unknown syscall: " + symtab().get(shape) + " " + symtab().get(s));
            }
        }


        void parse(const metalan::Symlist& prog) {
  
            p_i = prog.syms.begin();
            p_e = prog.syms.end();


            while (p_i != p_e) {

                const std::string& op_name = metalan::symtab().get(p_i->sym);

                /*
                  std::cout << "!" << op_name << std::endl;
                  for (const auto& s : labelstack) {
                      std::cout << s.label.print() << std::endl;
                      for (const auto& s2 : s.shapestack) 
                          std::cout << " " << symtab().get(s2);
                      std::cout << std::endl;
                  }
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

                } else if (op_name == "_get_all_fields") {
                    get_all_fields();
                
                    ++p_i;
                    continue;

                } else if (op_name == "_call_or_syscall") {
                    call_or_syscall(false);

                    ++p_i;
                    continue;

                } else if (op_name == "_tailcall_or_syscall") {
                    call_or_syscall(true);

                    ++p_i;
                    continue;

                } else if (op_name == "_asmcall") {
                    asmcall();

                    ++p_i;
                    continue;
                }


                auto& c = code.codes[label(false)];

                Opcode op;

                bool raw_push = false;

                if (op_name == "PUSH_RAW") {
                    raw_push = true;
                    op.op = PUSH;

                } else {
                    op.op = opcodecode(op_name);
                }

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

                    if (raw_push) {
                        op.arg = string_to_uint(metalan::symtab().get(p_i->sym));

                    } else if (arg_type == "Sym") {
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

        try {
            ctx.parse(prog);

        } catch (std::exception& e) {
            std::string msg = ("In function: " + 
                               (ctx.labelstack.empty() ? std::string("<toplevel>") : ctx.label().print()) + 
                               ": " + e.what());
            throw std::runtime_error(msg);
        }
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
