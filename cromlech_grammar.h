#ifndef __CROMLECH_GRAMMAR_H
#define __CROMLECH_GRAMMAR_H


#include <pegtl.hh>

namespace crom {

using namespace pegtl;


/* Typedef statements. */

struct symbol : seq< lower, 
                     star< sor< lower, upper, digit, one<'_'> > >
                     > {};

struct typenam : seq< upper,
                      star< sor< lower, upper, digit, one<'_'> > >
                      > {};

struct ns_typenam_l : sor< seq< symbol, string<':',':'>, typenam >,
                           typenam
                           > {};

struct int_l : seq< opt< one<'-'> >, 
                    plus< digit > 
                    > {};
  
struct real_l : sor< seq< int_l, one<'.'>, 
                          plus< digit > 
                          >,
                     seq< int_l, one<'e','E'>, int_l >
                     > {};

struct bool_l : sor< string<'\\','t','r','u','e'>,
                     string<'\\','f','a','l','s','e'>
                     > {};


template <char Q>
struct string_q_ : sor< not_one<Q, '\\'>,
                        seq< one<'\\'>,
                             sor< seq<xdigit, xdigit>,
                                  one<'\\', 'n', 't', 'r', Q>
                                  >
                             >
                        > {};


template <char Q>
struct string_l_ : seq< one<Q>,
                        must< ifapply< 
                                  star< string_q_<Q> >,
                                  a_string_literal
                                  >
                              >,
                        one<Q>
                        > {};

struct string_l : sor< string_l_<'"'>, string_l_<'\''> > {};

struct typename_or_literal : seq< sor< ifapply< ns_typenam_l, a_type >,
                                       ifapply< symbol, a_symbol_literal >,
                                       ifapply< int_l, a_int_literal >,
                                       ifapply< real_l, a_real_literal >,
                                       ifapply< bool_l, a_bool_literal >,
                                       string_l
                                       >,
                                  pad< one<':'>, space >,
                                  ifapply< symbol, a_struct_key, a_struct_val >
                                  > {};

struct struct_def : seq< ifapply< padr< one<'{'>, space >, 
                                  a_struct_start >,
                         star< seq< typename_or_literal,
                                    star<space>,
                                    opt< one<','> >,
                                    star<space>
                                    >
                               >,
                         ifapply< padl< one<'}'>, space >,
                                  a_struct_end >
                         > {};

struct type : sor< typenam, struct_def > {};

struct typdef : seq< string<'d','e','f','i','n','e'>,
                     plus<space>,
                     must< type >,
                     plus<space>,
                     string<'a','s'>,
                     plus<space>,
                     must< ifapply< typenam, a_define_type > >,
                     star<space>,
                     one<';'>
                     > {};


/* Function definition statements. */

struct expr_1;

struct ns_funname : sor< ifapply< seq< symbol , 
                                       string<':',':'>, 
                                       symbol 
                                       >, 
                                  a_setfun_ns >,
                         seq< one<'*'>, ifapply< symbol, a_setsyscall > >,
                         ifapply< symbol, a_setfun >
                         > {};

struct ns_typenam : sor< seq< symbol, string<':',':'>, typenam >,
                         typenam
                         > {};

struct literal : sor< ifapply< symbol, a_symbol_literal >,
                      ifapply< int_l, a_int_literal >,
                      ifapply< real_l, a_real_literal >,
                      ifapply< bool_l, a_bool_literal >,
                      string_l
                      > {};


struct funcall : seq< ns_funname,
                      pad< one<'$'>, space >,
                      expr_1
                      > {};

struct expr_e : sor< ifapply< one<'_'>, a_frameget >,
                     seq< padr< one<'('>, space >,
                          must< expr_1 >,
                          padl< one<')'>, space >
                          >,
                     ifapply< funcall, a_do_funcall >,
                     literal
                     > {};

struct expr_deref : seq< expr_e,
                         star< seq< pad< one<'.'>, space >,
                                    ifapply< symbol, a_varget >
                                    >
                               >
                         > {};

                    
struct expr_mul : seq< expr_deref,
                       star< seq< pad< ifapply< sor< one<'*'>, 
                                                     one<'/'>,
                                                     one<'%'>
                                                     >, a_setop
                                                >,
                                       space
                                       >,
                                  ifapply< expr_deref, a_do_op >
                                  >
                             >
                       > {};

struct expr_add : seq< expr_mul,
                       star< seq< pad< ifapply< sor< one<'+'>,
                                                     one<'-'> 
                                                     >, a_setop
                                                >,
                                       space
                                       >,
                                  ifapply< expr_mul, a_do_op >
                                  >
                             >
                       > {};

struct expr_and : seq< expr_add,
                       star< seq< pad< string<'&','&'>, 
                                       space >,
                                  expr_add
                                  >
                             >
                       > {};

struct expr_1 : seq< expr_and,
                     star< seq< pad< string<'|','|'>, space >,
                                expr_and
                                >
                           >
                     > {};

struct struct_e : seq< padr< one<'{'>, space >,
                       apply< a_fun_struct_s >,
                       star< seq< ifapply< expr_1, a_fun_struct_v >,
                                  opt< pad< one<','>, space > >
                                  >
                             >,
                       padl< one<'}'>, space >
                       > {};
                   
struct expr : sor< struct_e, expr_1 > {};

struct codeblock : seq< expr, 
                        star< seq< pad< one<','>, space >,
                                   expr 
                                   >
                              >
                        > {};


struct fun : seq< string<'f','u','n'>,
                  plus<space>,
                  must< seq< opt< one<'>'> >, 
                             ifapply< symbol, a_symbol_literal >
                             >
                  >,
                  plus<space>,
                  ifapply< ns_typenam, a_type >,
                  pad< string<'-','>'>, space >, 
                  ifapply< ns_typenam, a_type >,
                  plus<space>,
                  must< codeblock >,
                  must< pad< one<';'>, space > >,
                  apply< a_fun_end >
                  > {};


/* Toplevel. */

struct namspace : seq< string<'n','a','m','e','s','p','a','c','e'>,
                       plus<space>,
		       must< ifapply< symbol, a_set_namespace > >,
                       star<space>,
                       one<';'>
                       > {};


struct tunit : seq< star<space>, 
                    namspace,
                    star< pad< sor< seq< string<'/','/'>, until<eol> >, 
				    fun, 
				    typdef, 
				    namspace >, 
                               space > 
                          >,
                    star<space>,
                    must< eof >
                    > {};

}



#endif
