
*****
Intro
*****

Piccol is a simple, small, strict and statically-typed scripting 
extension language for embedding into a larger application.

Mission statement
=================

Why not Lua, Javascript, Scheme, Tcl, Python?

Existing extension languages (Lua is a great example) are wonderful, and I use them daily. 
But they are made for a specific purpose: to allow for easy and rapid prototyping 
of features, especially by people who are not professional programmers or not 
very familiar with the app they are trying to extend. 

Piccol is made to fill a very different niche. 

Existing languages are designed to make adding new features easy.
Piccol is designed to make breaking existing functionality is hard.

Piccol is a very strict, bondage-and-discipline, "hairshirt" language 
which has extensive compile-time checking and a limited feature set.

Piccol is also in the tradition of 'elegant' languages like Scheme, Prolog and Forth: it has 
a very tight, easy to grasp semantic and syntactic core and a macro mechanism for extending
syntax and adding sugar.

Language features of Piccol
===========================

 * It is meant to function *only* as an extension language in a host C++ program.
   It can't function stand-alone, since basic functionality is missing.

 * It has static typing at compile time.

 * *All* data is pass-by-value; there are no pointers. 
   (When data is passed to a function it is copied.)

 * All structures are fixed-size: there are no recursive structures,
   no arrays, no 'bytestring' datatype.

 * Thus, data structures are not implementable in Piccol and are
   meant to be implemented in the C++ part of your program.

 * Strings are supported only as immutable entries in a symbol table.
   (Represented as integer indexes into the symbol table at runtime.)

 * There is *no* garbage collection. (Garbage collection is unneded because all function arguments
   are passed by value and recursive structures are not allowed.)

 * Piccol code is purely functional and referentially-transparent.
   (Purity of C++ callbacks called from Piccol is not enforced, though.)

 * There are no looping primitives; Piccol fully supports tail call 
   optimization instead.

 * Functions are not first-class objects. (Functions are not 'objects'
   at all, since there is no concept of a function pointer.)
   Inline lambda-functions (as a control construct) are fully supported, however.

 * Types are nominal, not structural. That is, ``def { port:Int } NetAddr`` and 
   ``def { no:Int } TelephoneNo`` would be considered different types by the 
   compiler, even though their structure is the same.

 * Functions are polymorphic on their *return* type, not only on their input type.
   That is, ``get_foo Void->Real`` and ``get_foo Void->Int`` are different functions
   with different implementations.

 * There is no type inference, types are fully specified by the programmer at the 
   point where the function is called.
   (This is partly for readability, partly because of the
   previous bullet point about polymorphic return types.)

 * No type coercion whatsoever. ``1 + 0.5`` is a type error. 
   (Even more: ``1 + 2u`` is *also* a type error; write ``uint(1) + 2u`` instead.)

 * There are no variables or scopes; Piccol is a stack-based language and
   a function can only access the top value on the stack.

 * It supports non-linear control logic. Any function can either succeed and
   return a value or fail. Failure can be controlled and acted upon at any
   point in the execution of a program.

 * It has a non-standard syntax inspired by ML, Prolog and Forth. (Not a curly-brace syntax!)

Implementation details of Piccol
================================

 * It *strongly* adheres to a "layered-cake" compiler architecture; you can use (or change)
   any aspect of the language in isolation without hard dependencies on the other parts.
   For example, you can change the language syntax by tweaking a text file; rebuilding the
   compiler is not even necessary. The module system is a separate layer on top of the
   basic VM which can be easily ignored or replaced. The VM itself has few dependencies
   on the language, and can be easily replaced by a JIT compiler, for example.

 * It includes a compile-time macro preprocessor which is language-agnostic. 
   (It is a simple preprocessor which transforms strings to strings.)
   However, macro definitions are full-featured BNF grammar specifications which 
   can be composed, nested and inherited.
   
 * It includes an involved module system. Modules are not simply namespaces; rather, 
   each module is a separate, sandboxed VM with a defined interface. 
   Communication between modules can only happen via the defined interfaces and
   is routed through a C++ layer.
   (Thus, a module could easily be accessed remotely via an RPC mechanism, via 
   serialization with a separate process, a module could be replaced with 
   a VM running code written in an entirely different language, etc.
   There are many theoretical possibilities.)

 * FFI between Piccol code and C++ callbacks it meant to be as straightforward
   and unobtrusive as possible. (With variable success. :))

 * There are no baked-in data structures, though a few useful ones are 
   provided out-of-the-box, usable by including an extra .h file.

 * It is written in portable C++11 with no OS dependencies. 
   (Tested with the latest gcc.) 
   There are no bindings for pure C at the moment.

Wait, what?
===========

Piccol is a real language and has been extensively field-tested on a real, moderately complex program.




