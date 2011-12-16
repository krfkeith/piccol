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
struct one_tuple_l : seq< ifapply< T, a_tuple_element >,
                          opt< seq< pad< one<'*'>, space >,
                                    ifapply< int_l, a_tuple_clone >
                                    >
                               >
                          > {};

template <typename T>
struct one_struct_l : seq< ifapply< symbol, a_struct_key >,
                           pad< one<'='>, space >,
                           must< T >, apply< a_struct_val >
                           > {};

template <typename S, typename P, typename E, typename ACTS, typename ACTE>
struct commasep_ : seq< padr< ifapply< S, ACTS >, space >,
                        star< seq< P,
                                   star<space>,
                                   opt< one<','> >,
                                   star<space>
                                   >
                              >,
                        padl< ifapply< E, ACTE >, space > 
                        > {};

template <typename T>
struct tuple_ : commasep_< one<'['>, one_tuple_l<T>, one<']'>, a_tuple_start, a_tuple_end > {};

template <typename T>
struct struct_ : commasep_< one<'{'>, one_struct_l<T>, one<'}'>, a_struct_start, a_struct_end > {};

struct tuple_l : tuple_<literal> {};
struct struct_l : struct_<literal> {};

struct literal_pod : sor< ifapply< symbol, a_symbol_literal >,
                          ifapply< real_l, a_real_literal >,
                          ifapply< int_l, a_int_literal >,
                          ifapply< bool_l, a_bool_literal >,
                          string_l
                          > {};

struct literal : sor< literal_pod,
                      ifapply< tuple_l, a_tuple_literal >,
                      ifapply< struct_l, a_struct_literal >
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

struct type : sor< ifapply< string<'S','y','m','b','o','l'>, a_type<SYMBOL_TYPE> >,
                   ifapply< string<'I','n','t'>, a_type<INT_TYPE> >,
                   ifapply< string<'R','e','a','l'>, a_type<REAL_TYPE> >,
                   ifapply< string<'B','o','o','l'>, a_type<BOOL_TYPE> >,
                   ifapply< string<'S','t','r','i','n','g'>, a_type<STRING_TYPE> >,
                   ifapply< typenam, a_type<CUSTOM_TYPE> >,
                   ifapply< tuple_t, a_type<TUPLE_TYPE> >,
                   ifapply< struct_t, a_type<STRUCT_TYPE> >
                   > {};



/* The pattern match language. */

struct patternmatch;

struct tuple_pm : tuple_<patternmatch> {};
struct struct_pm : struct_<patternmatch> {};

struct patternmatch : sor< seq< type, one<':'>, must< ifapply< symbol, a_vardef > > >,
                           literal_pod,
                           ifapply< tuple_pm, a_match_tuple >,
                           ifapply< struct_pm, a_match_struct >
                           > {};

/* The pattern application language. */

struct patternappl;

struct tuple_pa : tuple_<patternappl> {};
struct struct_pa : struct_<patternappl> {};

struct patternappl : sor< seq< one<':'>, ifapply< must< symbol >, a_varget > >,
                          literal_pod ,
                          ifapply< tuple_pa, a_tuple_literal >,
                          ifapply< struct_pa, a_struct_literal >
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
