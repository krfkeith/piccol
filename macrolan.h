
#include "metalan_prime.h"


namespace macrolan {


struct Macrolan {

    metalan::MetalanPrime mlp;
    metalan::Symlist code;

    std::unordered_map<metalan::Sym,metalan::Symlist> macros;


    Macrolan(const std::string& c) {
        code.parse(c);
    }

    std::string parse(const std::string& inp) {
        
        metalan::Symlist code_ = code;
        metalan::Symlist o = mlp.parse(code_, inp);

        std::string ret;

        auto b = o.syms.begin();
        auto e = o.syms.end();

        while (b != e) {

            const std::string& op = metalan::symtab().get(b->sym);

            ++b;
            if (b == e)
                throw std::runtime_error("Unexpected end of input");

            if (op == "TEXT") {
                ret += metalan::symtab().get(b->sym);

            } else if (op == "APPLY") {
                auto tmp = macros.find(b->sym);

                const std::string& rulename = metalan::symtab().get(b->sym);

                if (tmp == macros.end())
                    throw std::runtime_error("Unknown macro applied: " + rulename);

                ++b;
                if (b == e)
                    throw std::runtime_error("End of input in 'APPLY'");

                metalan::MetalanPrime map;
                metalan::Symlist sl = tmp->second;

                try {
                    ret += map.parse(sl, metalan::symtab().get(b->sym), false,
                                     rulename).print_raw();

                } catch (std::exception& e) {
                    throw std::runtime_error("Error while applying macro '" + rulename +
                                             std::string("': ") + e.what());
                }

            } else if (op == "DEFINE") {

                auto tmp = macros.find(b->sym);

                if (tmp != macros.end())
                    throw std::runtime_error("Macro defined twice: " + metalan::symtab().get(b->sym));

                metalan::Symlist& newmacro = macros[b->sym];

                ++b;
                if (b == e)
                    throw std::runtime_error("End of input in 'DEFINE'");

                while (1) {
                    if (b->sym == metalan::symtab().get(":"))
                        break;

                    auto tmp = macros.find(b->sym);

                    if (tmp == macros.end())
                        throw std::runtime_error("Unknown macro in definition: " + metalan::symtab().get(b->sym));

                    newmacro.syms.insert(newmacro.syms.end(), tmp->second.syms.begin(), tmp->second.syms.end());

                    ++b;
                    if (b == e)
                        throw std::runtime_error("End of input in 'DEFINE'");
                }

                ++b;
                if (b == e)
                    throw std::runtime_error("End of input in 'DEFINE'");

                metalan::Symlist newcode;
                newcode.parse(metalan::symtab().get(b->sym));

                newmacro.syms.splice(newmacro.syms.end(), newcode.syms);
            }


            ++b;
            if (b == e) {
                break;
            }
        }

        return ret;
    }
};


}

