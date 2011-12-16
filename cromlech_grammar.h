#ifndef __CROMLECH_GRAMMAR_H
#define __CROMLECH_GRAMMAR_H


#include <pegtl.hh>

namespace crom {

using namespace pegtl;

/* The literal definition language. */

struct literal;

struct symbol : seq< lower, 
                     star< sor< lower, upper, digit, one<'_'> > >
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
                                  string_q_<Q>,
                                  a_string_literal
                                  >
                              >,
                        one<Q>
                        > {};

struct string_l : sor< string_l_<'"'>, string_l_<'\''> > {};


template <typename T>
struct one_tuple_l : seq< T,
                          opt< seq< pad< one<'*'>, space >,
                                    int_l
                                    >
                               >
                          > {};

template <typename T>
struct one_struct_l : seq< symbol, 
                           pad< one<'='>, space >,
                           must< T >
                           > {};

template <typename S, typename P, typename E>
struct commasep_ : seq< padr< S, space >,
                        star< seq< P,
                                   star<space>,
                                   opt< one<','> >,
                                   star<space>
                                   >
                              >,
                        padl< E, space > 
                        > {};

template <typename T>
struct tuple_ : commasep_< one<'['>, one_tuple_l<T>, one<']'> > {};

template <typename T>
struct struct_ : commasep_< one<'{'>, one_struct_l<T>, one<'}'> > {};

struct tuple_l : tuple_<literal> {};
struct struct_l : struct_<literal> {};

struct literal_pod : sor< ifapply< symbol, a_symbol_literal >,
                          ifapply< real_l, a_real_literal >,
                          ifapply< int_l, a_int_literal >,
                          ifapply< bool_l, a_bool_literal >,
                          string_l
                          > {};

struct literal : sor< literal_pod,
                      tuple_l, 
                      struct_l 
                      > {};


/* The type definition language. */

struct type;

struct typenam : seq< upper,
                      star< sor< lower, upper, digit, one<'_'> > >
                      > {};

struct type_or_literal : sor< seq< padr< one<'<'>, space >,
                                   literal,
                                   padl< one<'>'>, space > 
                                   >,
                              type
                              > {};

struct tuple_t : tuple_<type_or_literal> {};
struct struct_t : struct_<type_or_literal> {};

struct type : sor< string<'S','y','m','b','o','l'>,
                   string<'I','n','t'>,
                   string<'R','e','a','l'>,
                   string<'B','o','o','l'>,
                   string<'S','t','r','i','n','g'>,
                   typenam,
                   tuple_t,
                   struct_t
                   > {};



/* The pattern match language. */

struct patternmatch;

struct tuple_pm : tuple_<patternmatch> {};
struct struct_pm : struct_<patternmatch> {};

struct patternmatch : sor< seq< type, one<':'>, must< symbol>  >,
                           literal_pod,
                           tuple_pm,
                           struct_pm
                           > {};

/* The pattern application language. */

struct patternappl;

struct tuple_pa : tuple_<patternappl> {};
struct struct_pa : struct_<patternappl> {};

struct patternappl : sor< seq< one<':'>, symbol >,
                          literal_pod ,
                          tuple_pa,
                          struct_pa
                          > {};


/* The function call language. */

struct expr;

struct funcall : seq< symbol, 
                      plus<space>,
                      expr
                      > {};

struct expr_e : sor< seq< padr< one<'('>, space >,
                          must< expr >,
                          padl< one<')'>, space >
                          >,
                     funcall,
                     patternappl
                     > {};
                    

struct expr_add : seq< expr_e,
                       star< seq< pad< sor< one<'+'>,
                                            one<'-'>
                                            >, 
                                       space
                                       >,
                                  expr_e
                                  >
                             >
                       > {};

struct expr_mul : seq< expr_add,
                       star< seq< pad< sor< one<'*'>, 
                                            one<'/'>,
                                            one<'%'>
                                            >, space
                                       >,
                                  expr_add
                                  >
                             >
                       > {};

struct expr_and : seq< expr_mul,
                       star< seq< pad< string<'&','&'>, space >,
                                  expr_mul
                                  >
                             >
                       > {};

struct expr : seq< expr_and,
                   star< seq< pad< string<'|','|'>, space >,
                              expr_and
                              >
                         >
                   > {};
                   

struct codeblock : plus< seq< pad< expr, space >,
                              must< one<';'> >
                              >
                         > {};


struct fun : seq< string<'f','u','n'>,
                  plus<space>,
                  must< symbol >,
                  plus<space>,
                  padr<patternmatch, space>,
                  padr<string<'=','>'>, space>,
                  must< codeblock >,
                  must< padl< one<';'>, space > >
                  > {};

/* Toplevel. */

struct namspace : seq< string<'n','a','m','e','s','p','a','c','e'>,
                       plus<space>,
                       must< symbol >,
                       star<space>,
                       one<';'>
                       > {};

struct typdef : seq< string<'d','e','f','i','n','e'>,
                     plus<space>,
                     must< type_or_literal >,
                     plus<space>,
                     string<'a','s'>,
                     plus<space>,
                     must< typenam >,
                     star<space>,
                     one<';'>
                     > {};

struct tunit : seq< star<space>, 
                    namspace,
                    star< pad< sor< fun, typdef, namspace >, 
                               space > 
                          >,
                    star<space>,
                    must< eof >
                    > {};

}



#endif
