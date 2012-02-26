#ifndef __NANOM_ASM_H
#define __NANOM_ASM_H

#include "nanom_vm.h"

namespace nanom {


enum opcode_arg_t {
    NONE = 0,
    VAL,
    UINT,
    ADDR
};


inline Int string_to_int(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    Int ret = ::strtoll((char*)b, (char**)&e, 0);

    if (*e != end) 
        throw std::runtime_error("Invalid signed integer number: " + arg);
    return ret;
}

inline UInt string_to_uint(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    UInt ret = ::strtoull((char*)b, (char**)&e, 0);

    if (*e != end) 
        throw std::runtime_error("Invalid unsigned integer number: " + arg);
    return ret;
}

inline Real string_to_real(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    Real ret = ::strtod((char*)b, (char**)&e);

    if (*e != end) 
        throw std::runtime_error("Invalid double number: " + arg);
    return ret;
}


struct Assembler {
    std::unordered_map<std::string,Val> const_map;
    std::unordered_map<std::string,size_t> addr_map;

    struct opcodes_t {
        std::unordered_map< std::string, std::pair<op_t,opcode_arg_t> > opcodes_map;

        opcodes_t& operator()(const std::string& name, op_t op, opcode_arg_t a) {
            opcodes_map[name] = std::make_pair(op, a);
            return *this;
        }
    };

    opcodes_t opcodes_map;

    Vm& vm;

    Assembler(Vm& v) : vm(v) {

        opcodes_map
            ("PUSH", PUSH, VAL)
            ("TO_HEAP", TO_HEAP, UINT)
            ("COPY_TO_HEAP", COPY_TO_HEAP, UINT)
            ("FROM_HEAP", FROM_HEAP, UINT)
            ("SIZE_HEAP", SIZE_HEAP, UINT)
            ("POP", POP, NONE)
            ("CMP_INT", CMP_INT, NONE)
            ("CMP_UINT", CMP_UINT, NONE)
            ("CMP_REAL", CMP_REAL, NONE)
            ("CALL", CALL, ADDR)
            ("CALL_IF", CALL_IF, ADDR)
            ("JUMP", JUMP, ADDR)
            ("JUMP_IF", JUMP_IF, ADDR)
            ("JUMP_CHECK_IF", JUMP_CHECK_IF, ADDR)
            ("RET", RET, NONE)
            ("RET_IF", RET_IF, NONE)
            ("SYSCALL", SYSCALL, UINT)
            ("EXIT", EXIT, NONE)
            ("INT_TO_REAL", INT_TO_REAL, NONE)
            ("REAL_TO_INT", REAL_TO_INT, NONE)
            ("ADD_INT", ADD_INT, NONE)
            ("SUB_INT", SUB_INT, NONE)
            ("MUL_INT", MUL_INT, NONE)
            ("DIV_INT", DIV_INT, NONE)
            ("ADD_UINT", ADD_UINT, NONE)
            ("SUB_UINT", SUB_UINT, NONE)
            ("MUL_UINT", MUL_UINT, NONE)
            ("DIV_UINT", DIV_UINT, NONE)
            ("ADD_REAL", ADD_REAL, NONE)
            ("SUB_REAL", SUB_REAL, NONE)
            ("MUL_REAL", MUL_REAL, NONE)
            ("DIV_REAL", DIV_REAL, NONE)
            ("BAND", BAND, NONE)
            ("BOR", BOR, NONE)
            ("BNOT", BNOT, NONE)
            ("BXOR", BXOR, NONE)
            ("BSHL", BSHL, NONE)
            ("BSHR", BSHR, NONE)
            ;
    }

    void vm_run(const std::string& label) {

        auto i = addr_map.find(label);

        if (i == addr_map.end())
            throw std::runtime_error("Unknown address label: " + label);

        nanom::vm_run(vm, i->second);
    }


    void register_const(const std::string& name, Val v) {
        const_map[name] = v;
    }

private:


    std::string chomp(const std::string& arg_) {
        std::string arg;
        for (auto c : arg_) {
            if (!::isspace(c)) {
                arg += c;
            }
        }
        return arg;
    }

    Val process_val(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as value.");

        Val ret;

        unsigned char tag = arg[arg.size()-1];

        if (arg[0] == '$') {
            arg = arg.substr(1);
            auto i = const_map.find(arg);

            if (i == const_map.end()) 
                throw std::runtime_error("Unknown value constant: " + arg);

            return i->second;

        } else if (tag == 'f' || tag == 'F') {
            ret.real = string_to_real(arg, tag);
        } else if (tag == 'u' || tag == 'U') {
            ret.uint = string_to_uint(arg, tag);
        } else {
            ret.inte = string_to_int(arg, '\0');
        }

        return ret;
    }

    UInt process_uint(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as uint.");

        if (arg[0] == '$') {
            arg = arg.substr(1);
            auto i = const_map.find(arg);

            if (i == const_map.end()) 
                throw std::runtime_error("Unknown value constant: " + arg);

            return i->second.uint;
        }

        return string_to_uint(arg, '\0');
    }

    size_t process_addr(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as address.");

        if (arg[0] == '+') {
            return vm.code.size() - 1 + process_uint(arg.substr(1));

        } else if (arg[0] == '-') {
            return vm.code.size() - 1 - process_uint(arg.substr(1));

        } else {
            auto i = addr_map.find(arg);

            if (i == addr_map.end())
                throw std::runtime_error("Unknown address label: " + arg);

            return i->second;
        }
    }

    std::string::const_iterator process_string(std::string& val, std::string::const_iterator i, 
                                               std::string::const_iterator e) {

        enum {
            IN_START,
            IN_STRINGQ,
            IN_STRINGDQ,
            IN_QUOTE
        } state = IN_START, oldstate;

        while (i != e) {
            unsigned char c = *i;

            switch (state) {

            case IN_START: {
                if (::isspace(c)) {
                    // Nothing

                } else if (c == '\'') {
                    state = IN_STRINGQ;

                } else if (c == '"') {
                    state = IN_STRINGDQ;

                } else {
                    throw std::runtime_error("Invalid string constant: string needs to be quoted");
                }
                break;
            }

            case IN_STRINGQ: 
            case IN_STRINGDQ: {
                if (c == '\\') {
                    oldstate = state;
                    state = IN_QUOTE;

                } else if ((state == IN_STRINGQ && c == '\'') ||
                           (state == IN_STRINGDQ && c == '"')) {
                    ++i;
                    return i;

                } else {
                    val += c;
                }
                break;
            }

            case IN_QUOTE: {
                if (c == 't') {
                    val += '\t';

                } else if (c == 'n') {
                    val += '\n';

                } else {
                    val += c;
                }

                state = oldstate;
                break;
            }
            }

            ++i;
        }

        throw std::runtime_error("End of input while parsing string constant");
    }

    
    std::string::const_iterator process_token(std::string& s, bool toup,
                                              std::string::const_iterator i,
                                              std::string::const_iterator e) {

        if (i == e)
            throw std::runtime_error("Premature end of input");

        bool state = false;

        while (i != e) {
            if (::isspace(*i)) {
                if (state) {
                    return i;
                } else {
                    // Nothing.
                }

            } else {
                state = true;
                if (toup) {
                    s += ::toupper(*i);
                } else {
                    s += *i;
                }
            }
            ++i;
        }

        if (!state)
            throw std::runtime_error("Premature end of input");

        return i;
    }
                    


    std::string::const_iterator process_directive(std::string::const_iterator i,
                                                  std::string::const_iterator e) {

        std::string name;
        i = process_token(name, true, i, e);

        if (name == ".CONST") {
            std::string cname;
            std::string val;
            i = process_token(cname, false, i, e);
            i = process_token(val, false, i, e);

            const_map[cname] = process_val(val);

        } else if (name == ".LABEL") {
            std::string label;
            i = process_token(label, false, i, e);

            addr_map[label] = vm.code.size();

        } else if (name == ".SYMBOL") {
            std::string sym;
            std::string cell;

            i = process_string(sym, i, e);
            i = process_token(cell, false, i, e);

            vm.symtab[process_uint(cell)] = sym;

        } else if (name == ".SYMCONST") {

            std::string sym;
            std::string cname;
            std::string val;
            i = process_string(sym, i, e);
            i = process_token(cname, false, i, e);
            i = process_token(val, false, i, e);

            UInt uival = process_uint(val);
            const_map[cname] = uival;
            vm.symtab[uival] = sym;

        } else {
            throw std::runtime_error("Invalid directive: '" + name + "'");
        }

        return i;
    }

    std::string::const_iterator process_opcode(std::string::const_iterator i,
                                               std::string::const_iterator e) {



        std::string opcode;
        std::string arg;

        enum {
            IN_OPCODE,
            IN_ARG
        } state = IN_OPCODE;

        bool done = false;

        while (!done && i != e) {

            switch (state) {

            case IN_OPCODE: {
                if (*i == '(') {
                    state = IN_ARG;

                } else if (::isspace(*i)) {
                    done = true;

                } else {
                    if (::isalpha(*i)) {
                        opcode += ::toupper(*i);
                    } else {
                        opcode += *i;
                    }
                }
                break;
            }
             
            case IN_ARG: {
                if (*i == ')') {
                    done = true;

                } else if (::isspace(*i)) {
                    // Nothing.

                } else {
                    arg += *i;
                }
                break;
            }
            }

            ++i;
        }
        

        auto ix = opcodes_map.opcodes_map.find(opcode);

        if (ix == opcodes_map.opcodes_map.end())
            throw std::runtime_error("Unknown opcode: '" + opcode +"'");

        vm.code.push_back(Opcode());
        Opcode& opc = vm.code.back();

        opc.op = ix->second.first;

        switch (ix->second.second) {
        case VAL:
            opc.arg = process_val(arg);
            break;
        case UINT:
            opc.arg.uint = process_uint(arg);
            break;
        case ADDR:
            opc.arg.uint = process_addr(arg);
            break;
        }

        return i;
    }



public:

    void assemble(const std::string& s) {

        std::string::const_iterator i = s.begin();
        std::string::const_iterator e = s.end();

        try {

            while (i != e) {

                if (::isspace(*i)) {
                    ++i;

                } else if (*i == '.') {
                    i = process_directive(i, e);

                } else {
                    i = process_opcode(i, e);
                }
            }

        } catch (std::exception& ex) {

            std::string rest(i, e);

            if (rest.size() > 100) {
                rest = rest.substr(0, 100);
                rest += "...";
            }

            throw std::runtime_error(ex.what() + std::string("; error at: \"") + rest + "\"");
        }
    }
    
};


}

#endif
