
#include "metalan_prime.h"


namespace macrolan {


struct Macrolan {

    metalan::MetalanPrime mlp;
    metalan::Symlist code;

    std::unordered_map<metalan::Sym,metalan::Symlist> macros;


    Macrolan(const std::string& c) {
        code.parse(c);
    }

    std::string apply(metalan::Symlist::list_t::iterator& b, 
                      metalan::Symlist::list_t::iterator e) {

        static metalan::Sym apply_ = metalan::symtab().get("APPLY");
        static metalan::Sym arg = metalan::symtab().get("ARG");
        static metalan::Sym end = metalan::symtab().get("END");

        auto tmp = macros.find(b->sym);

        const std::string& rulename = metalan::symtab().get(b->sym);

        if (tmp == macros.end())
            throw std::runtime_error("Unknown macro applied: " + rulename);

        std::string argtext;

        // HACK
        std::vector<std::string> splices;

        while (b != e) {

            ++b;
            if (b == e)
                throw std::runtime_error("End of input in 'APPLY'");
            
            metalan::Sym op = b->sym;

            if (op == arg) {

                ++b;
                if (b == e)
                    throw std::runtime_error("End of input in 'APPLY'");
                
                argtext += metalan::symtab().get(b->sym);

            } else if (op == apply_) {

                ++b;
                if (b == e)
                    throw std::runtime_error("End of input in 'APPLY'");

                argtext += "<:[]:>";
                splices.push_back(apply(b, e));

            } else if (op == end) {

                metalan::MetalanPrime map;
                metalan::Symlist sl = tmp->second;

                try {
                    std::string ret = map.parse(sl, argtext, false, rulename).print_raw();

                    for (const auto& spl : splices) {
                        size_t n = ret.find("<:[]:>");

                        if (n == std::string::npos)
                            throw std::runtime_error("Sanity error, submacros are not marked in output");

                        ret = (ret.substr(0, n) + spl + ret.substr(n+6));
                    }

                    return ret;

                } catch (std::exception& e) {
                    throw std::runtime_error("Error while applying macro '" + rulename +
                                             std::string("': ") + e.what());
                }
            }
        }

        throw std::runtime_error("End of input in 'APPLY'");
    }

    std::string parse(const std::string& inp) {
        
        metalan::Symlist code_ = code;
        metalan::Symlist o = mlp.parse(code_, inp);

        std::string ret;

        auto b = o.syms.begin();
        auto e = o.syms.end();

        static metalan::Sym text = metalan::symtab().get("TEXT");
        static metalan::Sym apply_ = metalan::symtab().get("APPLY");
        static metalan::Sym define = metalan::symtab().get("DEFINE");

        while (b != e) {

            metalan::Sym op = b->sym;

            ++b;
            if (b == e)
                throw std::runtime_error("Unexpected end of input");

            if (op == text) {

                ret += metalan::symtab().get(b->sym);

            } else if (op == apply_) {

                ret += apply(b, e);

            } else if (op == define) {

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

