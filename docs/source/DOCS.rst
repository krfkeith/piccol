
.. highlight:: piccol

.. role:: raw-html(raw)
   :format: html


===============================
 The Piccol language reference
===============================

What follows is an informal but complete documentation on how to write Piccol programs.

Syntax
======

.. _lexical-structure:

Lexical structure
-----------------

Piccol has three kinds of identifiers:

.. index:: Type identifiers

**Type identifiers**

    Typenames *must* start with a capital letter, otherwise it is a syntax error! 
    Capital letters are the ASCII characters ``[A-Z]``. (No Unicode support yet.)

    Regex: ``[A-Z][A-Za-z0-9_]*``

    Examples: 
      * ``Int``
      * ``FooBar``
      * ``Type_1``

.. index:: Name identifiers

**Name identifiers** 

    Names *must* start with a lowercase letter.
    (Otherwise you get a syntax error, again.) 

    Regex: ``[a-z][A-Za-z0-9_]*`` 

    Examples:
      * ``foo``
      * ``aFunc``
      * ``a_11``

.. index:: Field identifiers

**Field identifiers**

    Field identifiers are like name identifiers, except they start with a
    backslash, ``'\'``. 

    Also, the special token ``\\`` (two backslashes) is parsed as a special field identifier.

    Examples:
      * ``\foo``
      * ``\x_AB``
      * ``\\``

.. index:: Syntax tokens

This is the complete list of all **tokens** that are used for parsing Piccol:

  ``\`` ``{`` ``}`` ``[`` ``]`` ``(`` ``)`` ``->`` ``:-`` ``=`` ``?`` ``;`` ``.`` ``/*`` ``*/`` ``#``

.. index:: Reserved words

This is the complete list of **reserved words** used in Piccol:

  ``def`` ``nil`` ``false`` ``true``

.. index:: Preprocessor tokens

These tokens are used in Piccol's **macro preprocessor**. They are also reserved, though not
strictly part of Piccol proper.

  ``<:`` ``:>`` ``<::`` ``::>``

.. index:: Whitespace

Whitespace is not significant, except as a delimiter.

Comments
--------

.. index:: Comments

Piccol has two types of comments: C-style ``/* ... */`` and script-style ``# ...``

Comments are parsed as whitespace.

.. _literal-values:

Literal values
--------------

.. index:: Literals

Piccol **literals** can be of four primitive types:
  
  * Real numbers. ("Doubles".)
  * Signed integers.
  * Unsigned integers.
  * Booleans.
  * Symbolic constants. (Like Lisp 'symbols' or Prolog 'atoms'. Piccol calls them 'symbols'.)

... index:: Real number literals

**Real numbers** can be of two forms:

  1. An optional ``'-'``, followed by a string of digits, followed by a ``'.'``, followed by a string of digits.
  (Note that the second 'string of digits' is required; you must write ``1.0`` instead of ``1.``)

    Regex: ``-?[0-9]+\.[0-9]+``

    Example: ``-1.0``
 
  2. An optional ``'-'``, followed by a string of digits, followed by the character ``'f'``.
  (Note: no space between the digits and the ``'f'``!)

    Regex: ``-?[0-9]+f``

    Example: ``3f``

.. index:: Unsigned integer literals

**Unsigned integers** have one form:
   
  A string of digits, followed by the mandatory character ``'u'``.
  (Note, again, no space between digits and ``'u'``.)

    Regex: ``[0-9]+u`` 

    Example: ``0u``

.. index:: Signed integer literals

**Signed integers** have one form:

  An optional ``-``, followed by a string of digits.
    Regex: ``-?[0-9]+`` 

    .. note::

       A number without an ``'f'`` or ``'u'`` suffix is parsed as a *signed* integer!

    Example: ``123``

.. index:: Booleans

**Booleans** are either the token ``true`` or the token ``false``.

.. index:: Symbolic constant literals, Special characters

**Symbolic constants** have two forms:

  1. A string of arbitrary characters delimited by ``'``. (Single quotes.) 

     .. note::

       Only single quotes are supported. Double quotes are not used in Piccol.

  2. The special token ``nil``, which denotes a special invalid symbol that cannot be represented as text.

Characters inside a symbolic constant can be quoted using the backslash, ``'\'``:

  * ``\n`` will expand to the newline symbol, like a ``'\n'`` in C.
  * ``\t`` will expand to the tab symbol, like a ``'\t'`` in C.
  * Any other symbol that follows a backslash will be used as-is in the symbolic constant.
    Thus, ``\'`` way to include a single quote in a symbolic constant.

.. note::

  No other characters have special meaning in a symbolic constant.

Thus, you can freely use newlines, non-printable symbols, Unicode, the null symbol, etc. in a symbolic constant.

Examples of symbolic constants:

  * ``''``
    The empty string.
  * ``'foo'``
  * ``'hello\nworld'``
  * ``'tab\tseparated\ttext'``
  * ``'Single quotes are \'quoted\' too.'``
  * ``'oops: \032'``

    .. note::

      The backslash in this symbol is ignored! It is equivalent to ``'oops: 032'``.

.. _typenames:
  
Typenames
---------

.. index:: Typenames

A **typename** is either a type identifier, or a tuple name.

Type identifiers -- see :ref:`lexical-structure` above.

Examples: 
  * ``Int`` 
  * ``MyType``
  * ``Type_1``.

.. index:: Tuples

A **tuple name** is a space-separated list (possibly empty) of typenames between ``[`` and ``]``.

For example:

  * ``[ Int ]``
  * ``[]``
  * ``[ Real Real MyType ]``
  * ``[Int [Foo Bar] [Atype Btype]]``

Type definitions
----------------

.. index:: Type definitions

A **type definition** starts with the keyword 'def' and ends in a semicolon, and has two forms: ::

  def [ <fields> ];

or ::

  def { <fields> } <typeIdentifier>;


Where <fields> is a (possibly empty) space-separated list of fields, where each field has the form ::

  <name> : <typename>

(Whitespace is optional.)

Here ``<name>`` is a name identifier, ``<typeIdentifier>`` is a type identifier (see :ref:`lexical-structure` above) and
``<typename>`` is a typename.

Examples of type definitions: ::

  def [ a:Int b:Int ];

  def { foo:FooType b:[Int Int] } Bar;

  def {} Void ;

  def { f1: [Int] 
        f2: [UInt] 
        f3: Data } AnotherType;

Function definitions
--------------------

.. index:: Function definitions

A **function definition** looks like this: ::

  <name> <typename> -> <typename> :- <expression> .

Where ``<name>`` is a name identifier (see :ref:`lexical-structure` above), and ``<typename>`` is a typename.

(We'll get to ``<expression>`` later, but note that a function body starts with a ``:-`` token and ends with a ``.`` (period).)

Examples of functions: ::

  foobar Void->MyType :-
    MyType{a=1}
  .

  some_func [Int Int]->Int :- \\ add->Int.

  maketwoints UInt-> [ Int Int ] :- 
    [ (one->Int) (two->Int) ].

.. _expression:
 
Expressions
-----------

.. index:: Expressions

Piccol has a postfix function call syntax, similar to stack-based languages like Forth and Factor.

An **expression** (remember that the body of a function is also an expression!) is a list of space-separated elements, where each element is one of:

  * A value
  * A function call
  * A lambda function call
  * A structure modifier
  * A control construct

You can think of Piccol functions as a kind of Unixy pipe: structures are pushed into the left side of the 
pipeline, and as you move right the original structure goes through function calls and is transformed.
There are also a few special control constructs to interrupt a pipeline or jump into the execution another one.

Examples of function bodies: ::

  1 add->UInt split->[UInt UInt] convert->Real print

  [ Foo{a=1} Bar{b=2} ] do_something->Foo ok->Bool ? 'OK' print ; 'Oops.' print

.. _values:

Values
------

.. index:: Values

A **value** is one of: 

  * A constructed tuple 
  * A constructed structure
  * A function argument field
  * A literal value

.. index:: Tuples

A **constructed tuple** looks like this: 

  ::

    [ <valuesOrParens> ]

  where ``<valuesOrParens>`` is a space-separated list of ``<valueOrParen>``, where each ``<valueOrParen>`` is either a 
    * value or an 
    * expression between ``(`` and ``)``.

  .. note::

    The parentheseses are required! If you want to assign an arbitrary expression to a field, you must enclose it
    in parentheses, otherwise the parser will get confused.

  Examples:
    * ``[1 2 3]``
    * ``[]``
    * ``['hello' 'world']``
    * ``[ \one \two ]``
    * ``[ (one->Int) (two->Int) (1 inc->Int) ]``

.. index:: Structures

A **constructed structure** looks like this: 

  ::

    <typename> { <fieldlist> }

  Where ``<fieldlist>`` is a space-separated list of key-value pairs like this: ::

    <name> = <valueOrParen>

  Examples:
    * ``MyType {firstfield=1 secondfield=2}``
    * ``Foo{ a = [] b = Bar{a=1 b=2} }``
    * ``Void{}``
    * ``SubExpr{ val=([1u 2u] complex_calculation->Real) }``

.. index:: Function arguments

A **function argument field** is a field identifier, see :ref:`lexical-structure` above.

For **literal values** see :ref:`literal-values` above.

.. note::

  This means that you *cannot* use parenthesised expressions in a function definition, neither can you nest
  parens! Parens are only used for setting fields in structures, they are not needed in an expression.

.. note::

  The above explanation is not quite correct. Parenthesised expressions differ from 'normal' expressions in
  function and lambda bodies in that parenthesised expressions *cannot* have control constructs. But don't worry, this
  is natural, since using a control construct while setting a structure field value makes no sense.

.. _function-calls:

Function calls
--------------

.. index:: Function calls

A **function call** looks like this: ::

  <name> -> <typename>

or like this: ::

  <name>

The second form is simply syntactic sugar; it is equivalent in all ways to writing ``<name> -> Void``.

Examples:
  * ``foo->Bar``
  * ``run->Result``
  * ``print``

    Note: equivalent to ``print->Void``

.. note:: 

  There is also a special set of 'inline assembly' functions, which have a ``$`` as the first character of ``<name>``.
  (Example: ``$add``, ``$mul``)
  They are built into the language and cannot be defined by the programmer. They'll be explained later.

.. _lambda-function-calls:

Lambda function calls
---------------------

.. index:: Lambda functions, Function calls

A **lambda function** in Piccol is an anonymous function that is defined in-line while an expression is parsed, and 
is immediately called at the corresponding place at runtime.

It looks like this: ::

  -> <typename> ( <functionBody> )

A ``<functionBody>`` is an expression; anything you can put in the body of function that's defined at toplevel you
can also put in the body of a lambda function.

Examples:
  * ``->Void('uh-oh' print)``
  * ``-> Int ( \v )``
  * ``-> Void ( 1 try_this; 2 try_another; 'oops' print fail )``

.. _structure-modifiers:

Structure modifiers
-------------------

.. index:: Structures, Structure modifiers

**Structure modifiers** look exactly like curly-brace structure constructors, except without the leading ``<typename>``: ::

  { <fieldlist> }

(See :ref:`values` above.)

Examples:
  * ``{a=1}``
  * ``{ x=(get_x->Int) y=(get_y->Int) }``

.. _structure-accessors:

Structure accessors
-------------------

.. index:: Structures, Structure accessors

**Structure accessors** are a way to access only a particular field of a structure. The syntax is similar to a
lambda function call, except with a field name instead of a typename to the right of the ``->``. ::

  -> <name>

Examples:
  * ``FooBar{foo=1 bar='hello'} ->bar``
  * ``[ [1 2] [3 4] ] ->a->b``


Control constructs
------------------

.. index:: Control constructs

There are exactly three **control constructs**. All three are reserved tokens in the language.

  * ``?``
  * ``;``
  * ``fail``

Semantics
=========

Types
-----

.. index:: Built-in types

Piccol types have several very restrictive properties:
  * They are always fixed-size. The size (in bytes) of a value of any given type is always constant at runtime.
  * Types cannot reference themselves recursively. 
  * Types cannot reference types that have not been defined yet.
  * There is no concept of a 'pointer' or 'reference' type. (All types are 'value types'.)

Piccol types are nominal, not structural. That means that if two types have different names they are considered
by the compiler to be two different types. (Even if they have exactly the same definition!)

Piccol has five basic, 'atomic' data types:

  * ``Int``, the signed 64-bit integer.
  * ``UInt``, the unsigned 64-bit integer.
  * ``Real``, the floating-point double.
  * ``Bool``, the boolean value. (Implemented internally as a 64-bit integer, 0 or 1.)
  * ``Sym``, the symbol. (Implemented internally as a 64-bit unsigned integer index into a symbol table.)
  * ``Void``, the empty type, which takes up no space and has no runtime representation.

They are equivalent to the following definitions, except for the fact that Piccol does not allow types to 
reference themselves. ::

  def { v:Int } Int;
  def { v:UInt } UInt;
  def { v:Real } Real;
  def { v:Bool } Bool;
  def { v:Sym } Sym;
  def { } Void;

.. index:: Tuples, Structures

There are two kinds of non-atomic composite types in Piccol: tuples and structures.

Internally, they are really the same. Tuple types are only a special kind of syntactic sugar so that
the programmer is not forced to invent meaningless typenames.

A tuple type definition looks like this: ::

  [ <typename>... ]

For example, ::

  [ Int MyType [Sym Sym ] ]

A definition of a tuple type is exactly equivalent to a corresponding structure type, so that ::

  def [ a:Int b:Int ];

is the exact same thing as ::

  def { a:Int b:Int } [ Int Int ];

Here the ``[ Int Int ]`` is treated as a type identifier by the compiler. 
(Even though it has spaces and brackets in the name!)

The same hold for using a tuple type, too: ::

  [ 12 89 'tag' ] foo->Int

is the exact same thing as ::

  [ Int Int Sym ] {a=12 b=89 c='tag'} foo->Int

.. note::

  Tuple types *must* be defined before use, just like structure types!

.. note::

  Because of how data types are defined in Piccol (no pointers, variables, call-by-value, recursive structures or closures)
  garbage collection is not needed. Piccol is a purely stack-based language.


Function evaluation
-------------------

.. index:: Function calls

Function names have three elements: 
  * the function name proper
  * the function's input type
  * the function's output type

Functions can be overloaded on any of the three elements; thus, two functions with the same name but
different input types are different and completely separate functions for the compiler.
(The same for functions that have the same name and the same input type, but different output types.)

A function can only have *one* input type and *one* output type.

.. note::

    If you want a function to accept several values, you will need to define a composite type. 
    Likewise, if you want a function that does not accept values or does not return them, use Void.

When a function is called it will either *succeed*, returning a type, or it will *fail*, not returning any value.

('Failure' is very similar to exceptions in other languages, except that Piccol failures don't have a type and
have a ligher implementation in the VM.)

When a function is called, (see :ref:`function-calls` syntax above) Piccol's syntax puts the input value on the *left* of the 
function name, while the function's return type must be specified on the *right*.

.. note::

   If a return type isn't specified then ``Void`` is assumed. There is no type inference in Piccol.

Examples:
  * ``[1 2] add->Int``
  * ``Foo{a=1} add->Int square->Real``
  * ``'hello' print exit``
    (Equivalent to ``'hello' print->Void exit->Void``)

Lambda functions
----------------

.. index:: Lambda functions

(See :ref:`lambda-function-calls` syntax above.)

A 'lambda function' is exactly the same as a 'normal' function, except that it 
  * doesn't have a name (it still has an input and return type, though!)
  * is defined inside the expression of another function or lambda function
  * is called at the same place where it is defined.

.. note::

  There is no concept of a function pointer or closure in Piccol. Thus, you can't return or pass a lambda function as
  an object. Lambda functions are only used as a way of structuring code, similar to curly-brace blocks in C.

Note that lambda functions will succeed or fail, just like 'normal' functions!

Structure modifiers
-------------------

.. index:: Structures, Structure modifiers

(See :ref:`structure-modifiers` syntax above.)

A structure modifier is really a special kind of syntactic sugar for changing fields in a structure.
It could be replaced by a lambda function, except that structure modifiers are implemented in a slightly
more efficient manner under the hood.

Here's an explanation by example:

Consider a structure like this: ::

  def { a:Int b:UInt c:Real } Foo;

Consider this code: ::

  Foo{ a=1 b=2u c=3f } { b=100u }

Then this code is fully equivalent to the following: ::

  Foo{ a= 1 b=2u c=3f } ->Foo( Foo{ a=\a b=100u c=\c } )

That is, the structure is accepted by the modifier, the fields specified in the modifier are changed, and 
the structure is then returned. (Without changing any other fields!)

Structure accessors
-------------------

.. index:: Structures, Structure accessors

(See :ref:`structure-accessors` syntax above.)

Likewise, a structure accessor is really a special kind of syntactic sugar for accessing a field in a structure.

It, also, could be replaced by a lambda function, except that structure accessor syntax is much terser.

For example, given the structure ::

  def { a:Int b:[UInt UInt] c:Real } Foo;

the following code will access the field named ``c``: ::

  Foo{ c=3.1415 } ->c

This code is equivalent to ::

  Foo{ c=3.1415 }->Real(\c)

You can also nest accessors in the obvious way: ::

  def { one:Int two:Int } Baz;
  def { baz:Baz } Bar;
  def { bar:Bar } Foo;

  Foo {} ->bar->baz->two

.. note::

  Unlike the equivalent lambda function, you don't need to specify the field's type when using the accessor.
  This makes using nested structures much more sane.


Constructing structures
-----------------------

.. index:: Structures

.. note::

  When constructing a structure, you don't have to specify all of its fields. The fields you leave out
  will be initialized with default values:

    * ``Int``: ``0``
    * ``UInt``: ``0u``
    * ``Real``: ``0f``
    * ``Bool``: ``false``
    * ``Sym``: ``nil``

For example: ::

  def {a:Int b:Sym c:Bool} Foo;

  Foo {}

Here the object that is constructed is really ``Foo { a=0 b=nil c=false }``

.. _funarg-fields:

Function argument fields
------------------------

.. index:: Function arguments, Field identifiers

Function argument fields is how you access the data of a function's input value.

(See 'field identifiers' above in :ref:`lexical-structure`.)

There are two ways to access the function's input value:

  * By accessing the contents of a certain field of the input value. (By using the ``\<fieldname>`` syntax.)
  * By accessing the input value as a whole. (The special token ``\\``)

Examples:

Here ``myfunc`` accepts a ``MyType`` value and returns the contents of its field ``a``. ::

  def { bar:Int } Foo;
  def { a:Int b:Foo } MyType;

  myfunc MyType->Int :- \a.

Here the field ``b`` is returned instead: ::

  myfunc2 MyType->Foo :- \b.

Here ``func`` accepts a ``MyType`` value and passes it to another function, ``frob``, returning nothing. ::

  func MyType->Void :- \\ frob.

Field identifiers work exactly the same for tuples as they do for structures: ::

  def [one:Int two:Int three:Int];

  foo [Int Int Int]->Int :- [ ([\one \two] add->Int) \three ] add->Int.

.. index:: Built-in types

.. note::

  Piccol provides several predefined types: ::

     def [ a:Int  b:Int  ];
     def [ a:UInt b:UInt ];
     def [ a:Bool b:Bool ];
     def [ a:Real b:Real ];
     def [ a:Sym  b:Sym  ];

     def [ v:Int ];
     def [ v:UInt ];
     def [ v:Sym ];
     def [ v:Bool ];
     def [ v:Real ];

  Keep that in mind when reading Piccol code.

Control constructs
------------------

.. index:: Control constructs

There are three control constructs in Piccol:

  * ``;``
  * ``fail``
  * ``?``

The body of a function (whether a 'normal' toplevel function or a lambda) can be segmented into several
so-called 'branches'.

Piccol has a simple runtime model: if, while evaluating a function body, any function fails, then the evaluation
of the current branch is aborted (discarding any intermediate results) and evaluation jumps to the next branch in list.
If all branches fail, then the function itself fails.
If a branch succeeds, then the functions successfully returns and the remaining branches are ignored.

Branches
--------

.. index:: Branches, Control constructs

Branches are delimited by the ``;`` token. 

Examples: ::

  one_of_3 [Int Int Int]->Void :- 
     \a foo ;
     \b foo ;
     \c foo.

Here, ``one_of_3`` tries to call the function ``foo`` with one of three arguments, returning successfully if any of the
three variants succeeds. If all three attempts at calling ``foo`` fail, then ``one_of_3`` itself fails. ::

  pick_one [Sym Sym]->Int :- [ 'foo' (\\->Sym(\a foo->Sym; \b foo->Sym)) ] bar->Int.

This is a more complex example, demonstrating branches inside a lambda function.
Here the function ``pick_one`` accepts two symbol values, passing either one of them to ``bar``, 
depending on whether ``foo`` succeeds or not.
Note that if ``foo`` fails for both values, then ``pick_one`` itself will fail. 
(Failures are bubbled up through the evaluation stack.) ::

  tryit Something->Void :- \\ foo; .

In this example the function ``tryit`` attempts to call ``foo`` with an argument of type ``Something``; if ``foo`` fails, then
``tryit`` succeeds anyways.

.. note::

  The function could be rewritten as ::

     tryit Something->Void :- \\ foo->Void; Void{} .

  The compiler inserts proper ``Void{}`` constructs where necessary and allows you to omit ``->Void`` for functions
  that return ``Void``.

  Note, however, that when returning a real type you need to annotate types and return values properly.

Example: ::

  tryit Thing->Int :- \\ convert->Int; 42.

This version of ``tryit`` tries to convert a value of type ``Thing`` to an ``Int``, using the ``convert`` function; if ``convert``
fails, then ``tryit`` returns the default value of ``42``.

Tests and fails
---------------

.. index:: Failure, Control constructs

The special token ``fail`` fails the current branch, unconditionally. It acts like a ``Void->Void`` function that always
fails.

It can be useful for implementing certain control structures. For example: ::

  try_all [Int Int Int]->Void :-
     \a foo fail ;
     \b foo fail ;
     \c foo fail ;
     .

Here ``try_all`` calls ``foo`` three times with three different ``Int`` arguments. ``try_all`` ignores failures of ``foo`` and
succeeds unconditionally.

The specials token ``?`` checks its boolean argument and succeeds or fails accordingly.
It acts like a ``Bool->Void`` function; its argument on the left must be a ``Bool``. If it is ``true``, then ``?`` succeds,
if it is ``false``, then ``?`` fails. There are no other effects.

Example: ::

  test Sym->Void :- \\ ok->Bool ? func_then; func_else.

This example shows how to implement a traditional 'if-then-else' control structure in Piccol.
If ``ok`` returns ``true``, then ``func_then`` will be called; if it returns ``false``, then ``func_else``.

Looping
-------

Piccol does not support any loop constructs, but it *does* support tail call optimisation properly.

If a function's expression ends with a call of another function, that call will be converted to an
optimized jump instruction for the VM. (i.e., that call will be effectively turned into a 'goto'.)

Any function or even lambda call will be optimized, not just a recursive call.

.. note:: 

  The tail call must happen at the end of a whole *function*; calls at the end of a *branch* cannot be optimized.

Examples: ::

  infiloop Void->Void :-
     'save me, I am an infinite loop. :(\n' print
     infiloop

This function will loop infinitely printing a message and will not consume stack or memory resources.
(Equivalent to a ``while (1) { ... }`` in C.)

Another example: ::

  forloop [Int Int]->Void :-
    <: \a >= \b :> ? ;
    'Looping.\n' print
    [ (<: \a + 1 :>) \b ] forloop.

This function implements a 'for' loop.

Example with lambda functions: ::

  tailcalls Int->Int :-
     \\->Int(\\ inc->Int(\\ inc->Int(\\ inc->Int))).

This (rather silly) function has three nested tail calls of lambda functions and one tail call of ``inc->Int``.
All four will all be optimised, converted to jumps instead of function calls.

Example with branches: ::

  tryit Int->Int :-
    \\ ok->Bool ? \\ ;
    \\ variant_a->Int tryit->Int ; 
    \\ variant_b->Int tryit->Int.

Here the *last* call of ``tryit->Int`` will be optimised, while the one above it will *not* be. 
(Since it is called at the end of a branch, not at the end of a function.)


Arithmetic and logic
====================

.. _infix-expressions:

Infix expressions
-----------------

.. index:: Arithmetic, Logic, Bitwise operations, Operators

The standard arithmetic, bit and boolean infix operators are realized in Piccol through macro expansion.

A macro is a block of text between the special tokens `<:` and `:>`. For example: ::

  add_mul [Int Int]->Int :- 
     <: \a + \a * \b :>
  .

When a macro is expanded it transforms the infix expression into a normal Piccol postfix expression.
This happens before any Piccol is parsed. (Similar to how the C preprocessor works.)

The example above would be turned into something resembling the following code: ::

  add_mul [Int Int]->Int :-
    [ \a ([\a \b] $mul) ] $add
  .

Here is the list of supported operators, in order of precedence. (The more tightly binding ones are listed first.)

.. csv-table:: 
  :header: "Precedence level","Operator","Accepted types","Operation"
  :widths: 10,20,20,45
  :delim: ;

  0;``(`` *EXPR* ``)``;  ;                   Parentheses can be used for grouping
  0;*LITERAL*;           ;                   Literal values or function argument fields
  0;``bool(`` *EXPR* ``)``; Int,UInt;           Converts the argument to a Bool
  0;``int(`` *EXPR* ``)``;  UInt,Real,Bool;     Converts the argument to an Int
  0;``uint(`` *EXPR* ``)``; Real,Int,Bool;      Converts the argument to a UInt
  0;``real(`` *EXPR* ``)``; Int,UInt;           Converts the argument to a Real    

  1;``!`` *X*;           Bool;               Boolean negation
  1;``-`` *X*;           Int,Real;           The negation of a number
  1;``~`` *X*;           UInt;               Binary 'not' (bit flipping)

  2;*X* ``&`` *Y*;       UInt;               Binary 'AND'
  2;*X* ``|`` *Y*;       UInt;               Binary 'OR'
  2;*X* ``^`` *Y*;       UInt;               Binary 'XOR'
  2;*X* ``<<`` *Y*;      UInt;               Bit shift left
  2;*X* ``>>`` *Y*;      UInt;               Bit shift right

  3;*X* ``*`` *Y*;       Int,UInt,Real;      Multiplication
  3;*X* ``/`` *Y*;       Int,UInt,Real;      Division
  3;*X* ``%`` *Y*;       Int,UInt;           'Modulo division'

  4;*X* ``+`` *Y*;       Int,UInt,Real;      Addition
  4;*X* ``-`` *Y*;       Int,UInt,Real;      Subtraction

  5;*X* ``==`` *Y*;      Int,UInt,Real,Sym;  Test for equality. (Returns Bool)
  5;*X* ``=`` *Y*;       Int,UInt,Real,Sym;  Synonym for ``==``
  5;*X* ``!=`` *Y*;      Int,UInt,Real,Sym;  Synonym for ``!(``*X*``==``*Y*``)``
  5;*X* ``<`` *Y*;       Int,UInt,Real;      'Less than'
  5;*X* ``<=`` *Y*;      Int,UInt,Real;      'Less than or equal to'
  5;*X* ``>`` *Y*;       Int,UInt,Real;      'Greater than'
  5;*X* ``>=`` *Y*;      Int,UInt,Real;      'Greater than or equal to'

  6;*X* ``&&`` *Y*;      Bool;               Boolean 'AND'. (Not short-circuited!)
  6;*X* ``||`` *Y*;      Bool;               Boolean 'OR'. (Not short-circuited!)

You can use parentheses for grouping expressions, so something like this ::

  <: (1+3)*(2+4) :> 

works as expected.

.. note::

  You can only use literals or function argument fields as values in infix expressions!

  (See :ref:`literal-values`, :ref:`funarg-fields`.)

That means, for example, that this code will result in a parse error: ::

  <: \a->b + 1 :>

(Because ``\a->b`` is an expression, not a function argument field; simply ``\a`` would be fine, though.)

You should rewrite this code using lambda functions: ::

  \a->Int(<: \b + 1 :>)

.. note::

  Arithmetic expressions **do not** do any type coercion.

An expression like ::

  <: 1 + 1.0 :>

is a type error. Moreover, ::

  <: \a + 1 :> 

is *also* a type error if ``\a`` is a ``UInt``. (Use ``<: \a + 1u :>`` instead.)

.. note::

  Arithmetic expressions are *not* parenthesised.

That means that this code ::

  Foo { a=<:\one + 1:> }

is a syntax error, since it will be expanded to something like ::

  Foo { a=[\one 1] $add }

The code should be rewritten with parentheses: ::

  Foo { a=(<: \one + 1 :>) }


Inline assembly functions
-------------------------

.. index:: Arithmetic, Logic, Bitwise operations, Operators

'Inline assembly' (see :ref:`function-calls` syntax) is a lower-level, postfix notation for arithmetic, logic and boolean 
operators.

Infix expression macros expand to expressions of inline assembly calls. 

Inline assembly functions are closely tied to the VM implementation and are hardcoded into the Piccol compiler.
They cannot be changed or overloaded.

Syntactically, inline assembly calls look like regular function calls, except that the return type is not 
specified, it is inferred from the argument type.

Names of inline assembly functions start with the ``$`` symbol.

Example: ::

  mul [Int Int]->Int :- [\a \b] $mul.

``$mul`` executes the 'INT_MUL' opcode and returns an ``Int``, since the arguments on the left side are ``Int``.

The following is a table of all inline assembly functions.

.. csv-table:: 
  :header: "Input type","Name","Output type","Operation"
  :widths: 10,10,10,40
  :delim: ,

  ``[ Int Int ]``,    ``add``,           ``Int``, Signed integer addition
  ``[ Int Int ]``,    ``sub``,           ``Int``, Signed integer subtraction
  ``[ Int Int ]``,    ``mul``,           ``Int``, Signed integer multiplication
  ``[ Int Int ]``,    ``div``,           ``Int``, Signed integer division
  ``[ Int Int ]``,    ``mod``,           ``Int``, Signed integer modulo
  ``Int``,            ``neg``,           ``Int``, Signed integer negation

  ``[ UInt UInt ]``,  ``add``,          ``UInt``, Unsigned integer addition
  ``[ UInt UInt ]``,  ``sub``,          ``UInt``, Unsigned integer subtraction
  ``[ UInt UInt ]``,  ``mul``,          ``UInt``, Unsigned integer multiplication
  ``[ UInt UInt ]``,  ``div``,          ``UInt``, Unsigned integer division
  ``[ UInt UInt ]``,  ``mod``,          ``UInt``, Unsigned integer modulo

  ``[ Real Real ]``,  ``add``,          ``Real``, Real number addition
  ``[ Real Real ]``,  ``sub``,          ``Real``, Real number subtraction
  ``[ Real Real ]``,  ``mul``,          ``Real``, Real number multiplication
  ``[ Real Real ]``,  ``div``,          ``Real``, Real number division
  ``Real``,           ``neg``,          ``Real``, Real number negation

  ``[ UInt UInt ]``,  ``band``,             ``UInt``, Bitwise 'AND'
  ``[ UInt UInt ]``,  ``bor``,              ``UInt``, Bitwise 'OR'
  ``[ UInt UInt ]``,  ``bnot``,             ``UInt``, Bitwise 'NOT'
  ``[ UInt UInt ]``,  ``bxor``,             ``UInt``, Bitwise 'XOR'
  ``[ UInt UInt ]``,  ``bshl``,             ``UInt``, Bit shift left
  ``[ UInt UInt ]``,  ``bshr``,             ``UInt``, Bit shift right

  ``[ Int Int ]``,    ``eq``,            ``Bool``, Signed integer equality
  ``[ Int Int ]``,    ``lt``,            ``Bool``, Signed integer 'less than'
  ``[ Int Int ]``,    ``lte``,           ``Bool``, Signed integer 'less than or equal to'
  ``[ Int Int ]``,    ``gt``,            ``Bool``, Signed integer 'greater than'
  ``[ Int Int ]``,    ``gte``,           ``Bool``, Signed integer 'greater than or equal to'

  ``[ UInt UInt ]``,  ``eq``,            ``Bool``, Unsigned integer equality
  ``[ UInt UInt ]``,  ``lt``,            ``Bool``, Unsigned integer 'less than'
  ``[ UInt UInt ]``,  ``lte``,           ``Bool``, Unsigned integer 'less than or equal to'
  ``[ UInt UInt ]``,  ``gt``,            ``Bool``, Unsigned integer 'greater than'
  ``[ UInt UInt ]``,  ``gte``,           ``Bool``, Unsigned integer 'greater than or equal to'

  ``[ Real Real ]``,  ``eq``,            ``Bool``, Real number equality
  ``[ Real Real ]``,  ``lt``,            ``Bool``, Real number 'less than'
  ``[ Real Real ]``,  ``lte``,           ``Bool``, Real number 'less than or equal to'
  ``[ Real Real ]``,  ``gt``,            ``Bool``, Real number 'greater than'
  ``[ Real Real ]``,  ``gte``,           ``Bool``, Real number 'greater than or equal to'

  ``[ Sym Sym ]``,  ``eq``,   ``Bool``, Comparison for equality of symbols

  ``[ Bool Bool ]``,  ``and``,           ``Bool``, Boolean 'AND'
  ``[ Bool Bool ]``,  ``or``,            ``Bool``, Boolean 'OR'
  ``Bool``,           ``not``,           ``Bool``, Boolean negation

  ``Int``,            ``to_real``,   ``Real``, Signed integer to real number conversion
  ``UInt``,           ``to_real``,   ``Real``, Unsigned integer to real number conversion
  ``Real``,           ``to_int``,    ``Int``, Real number to signed integer conversion
  ``Real``,           ``to_uint``,   ``UInt``, Real number to unsigned integer conversion. (Unsafe!!)

  ``Int``,            ``to_sym``,    ``Sym``, Converts ASCII code value to symbolic constant. (No Unicode support!!)
  ``UInt``,           ``to_sym``,    ``Sym``, Converts ASCII code value to symbolic constant. (No Unicode support!!)
            
  ``Int``,            ``to_uint``,           ``UInt``, Signed to unsigned integer conversion. (Unsafe!!)
  ``Int``,            ``to_bool``,           ``Bool``, Integer to boolean conversion
  ``UInt``,           ``to_int``,            ``Int``, Unsigned to signed integer conversion. (Unsafe!!)
  ``UInt``,           ``to_bool``,           ``Bool``, Unsigned integer to boolean conversion
  ``Bool``,           ``to_int``,            ``Int``, Boolean to signed integer conversion
  ``Bool``,           ``to_uint``,           ``UInt``, Boolean to unsigned integer conversion

  ``[ Int ]``,   ``noop``,   ``Int``, No-op; does absolutely nothing but useful for writing macros. 
  ``[ UInt ]``,  ``noop``,   ``UInt``, No-op
  ``[ Bool ]``,  ``noop``,   ``Bool``, No-op
  ``[ Real ]``,  ``noop``,   ``Real``, No-op
  ``[ Sym ]``,   ``noop``,   ``Sym``, No-op

  ``Bool``,           ``if``,               ``Void``, Equivalent to the ``?`` control construct.
  ``Void``,           ``fail``,              A special type of 'fail', Equivalent to the ``fail`` control construct. Does not return any type.


Macro preprocessor
==================

.. index:: Predefined macros

All Piccol source files are automatically preprocessed with :doc:`Macrolan </macrolan>` before parsing.

There are several macros predefined in Piccol, some to make it easier for the user to write other macros, 
some to be used directly as syntactic sugar.

The infix syntax for operators (:ref:`infix-expressions`) is a predefined as a macro named ``expr`` and ``default``.

(``expr`` if you want to use infix syntax expressions in your own macros, ``default`` so you can type ``<: ... :>`` 
instead of ``<:[expr] ... :>``.)

Some other predefined macros:

  * ``spaces``: matches zero or more whitespace characters.
  * ``ident``: matches name identifiers. (:ref:`lexical-structure`)
  * ``literals``: matches literals (:ref:`literal-values`) or function argument fields. (:ref:`funarg-fields`)
  * ``types``: matches typenames. (:ref:`typenames`)
  * ``type_canonical``: reformats a typename to 'canonical form', required in the C++ API.
  * ``body``: a helper macro for constructing parsers of sequences of code.
  * ``case``: the 'switch-case' construct, see below.
  * ``q``: a macro for quoting text in other macros.
  * ``seq``: a macro that implements the 'sequencer' idiom.

'case'
------

The 'case' macro has the following syntax: ::

  <:[case] <fieldref> :
      <literal> ? <expr> ;
      ...
   :>

where ``<fieldref>`` is a field identifier (:ref:`lexical-structure`), ``<literal>`` is a literal 
(:ref:`literal-values`) and ``<expr>`` is an arbitrary expression. (:ref:`expression`)

The trailing ``;`` is optional. Whitespace is optional too.

The ``case`` macro expands to something like the following: ::

  <: <fieldref> == <literal1> :> ? <body1> ;
  <: <fieldref> == <literal2> :> ? <body2> ;
  ...

You can also nest macros, which is especially useful with 'case': ::

  <:[case] \a : 
     'one' ? \b->Int( <:[case] \v : 
                            'one' ? 11 ; 
                            'two' ? 12 :> )
     'two' ? \b->Int( <:[case] \v :
                            'one' ? 21 ;
                            'two' ? 22 :> )
   :>

Note that the parser of this macro correctly detects ``;`` when it is part of a symbolic literal or 
lambda function. (Semicolons inside symbolic literals and parenthesised chunks of text are ignored.)

That is, the following code snippet is safe: it *does not* confuse the 'case' macro and does not
result in a parse error. ::

  <:[case] \a :
      1 ? 'text;with;semicolon' ;
      2 ? \b->Sym(\a ? 's1' ; 's2')
   :>

'q'
---

'q' is a special macro that allows you to 'quote' a block of text; that is, to insert verbatim some text into the output
of another macro.

A convoluted example: ::

  <:[case] \a :
     1 ? 'one' ;
     2 ? 'two' ;
    <:[q] \a is_three->Bool ? 'three' ; :>
     4 ? 'four' 
   :>

It is a very simple and straightforward macro that simply relies on the rules of nesting macro applications.

'seq'
-----

'seq' is a macro for taking advantage of the 'sequencer' idiom. 

'seq' has the following syntax: ::

  <:[case] <ident> => <exprs> => <typename> :>

or ::

  <:[case] <ident> => <exprs> :>

(The trailing ``=> <typename>`` is optional; if it is left off, then ``=> Void`` will be assumed.)

Here ``<ident>`` is a name of a function. (A name identifier, see :ref:`lexical-structure`.)

``exprs`` is a **comma-separated** list of expressions. (Commas inside symbol literals and parenthesised chunks of text
will be ignored.)

Here is an example of how this idiom works. The following macro ::

  <:[seq] fmt => 'One is ', one->Sym, ', two is ', two->Sym, '\n' => Sym :>

will be expanded to code that looks something like this: ::

              fmt_start
  'One is '   fmt
  one->Sym    fmt
  ', two is ' fmt
  two->Sym    fmt
  '\n'        fmt
              fmt_end->Sym

That is, the listed expressions will be interleaved with proper calls to ``fmt``, so that the list of expressions
is finally converted to a single value of type ``Sym``.

This idiom is a convienient way of passing a variable-sized list of values to a function.

It is also useful, for example, for implementing I/O and formatter functions. (Like ``printf`` in C and other languages.)

'do'
-----

'do' is a macro for expressing sequences of computations, but in a 'linear' fashion with variables, rather 
than the more ugly notation of nested expressions.

'do' has the following syntax: ::

  <:[do] <typename> -> <typename>

         <ident> = <expr>,
         <ident> = <expr>,
         ...
      =>
         <ident> = <expr>,
         ...
      =>
         <expr>
   :>

For example: ::

  <:[do] [Int Int] -> Int

     a = [\a \b] random->Int
     b = <: \a + \b :>
    =>
     a = <: \a * \b :>
    =>
     b = <: \a * \b :>
    => 
     [\a \b] foo->Int
   :>

This code will be transformed to: ::

  [Int Int] { a=([\a \b] random->Int)
              b=(<: \a + \b :>)
  } ->Int(
     \\ { a=(<: \a * \b :>) 
     } ->Int(
        \\ { b=(<: \a * \b :>)
        } ->Int(
           [\a \b] foo->Int
        )
     )
  )

The ``do`` expression is a kind of 'mini-program': a set of variables, a sequence of instructions, and
a resulting output value.

You can think of the first type (``[Int Int]`` in the example) as a kind of 
'memory bank', which holds the variables of your mini-program.

The second type (``Int``) is the type of the resulting output value.

Each line (separated by ``=>``) describes how to change variable values at each step of the mini-program's 
execution.

.. note::

  The last expression after the final ``=>`` is special! It describes how to convert the 'memory bank' into
  the resulting value.

.. note::

  The ``do`` macro is converted into a series of nested, tail-call-optimized function calls.
  This makes ``do`` macros particularly suitable for implementing loop-like functions.


