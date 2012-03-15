
/*
<<

intexpr :- \any:'Int' '+' \any:'Int' @''

>>


def [ Int Sym ];

bottles Int->Void :-
  [$v ' bottles of beer on the wall,'] print
  [$v ' bottles of beer,'] print
  'Take one down, pass it around,' print
  [ <: $v + 1 :>

*/

def [ a:Int b:Int ];
def [ a:Real b:Real ];

/* TODO! Check proper return type on exit. */

test Void->Int :- [3.25 ([1 (3.14 $to_int)] $add $to_real)] $add $to_int.

test3 Void->Void :- .

test2 Void->Int :- [3.14 5.5] $add $to_int.
