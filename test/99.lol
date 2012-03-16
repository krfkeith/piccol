
/*
<<

intexpr :- \any:'Int' '+' \any:'Int' @''

>>

*/

def [ a:Int b:Int ];
def [ a:Real b:Real ];

def [ a:Int b:Sym ];

bottles Void->Void :-
  99 bottles.

bottles Int->Void :-
  [ .v 'bottles of beer on the wall,'] print.

/*
bottles Int->Void :-
  [.v 0] $lte ? 
  'No more bottles of beer on the wall!' print
;
  [.v ' bottles of beer on the wall,'] print
  [.v ' bottles of beer,'] print
  'Take one down, pass it around,' print
  [ ([.v 1] $sub) ' bottles of beer on the wall.' ] print
  [.v 1] $sub bottles
.


bottles Int Void:
CALL_LIGHT bottles$1 Int Void
IF_NOT_FAIL{ EXIT }
CALL_LIGHT bottles$2 Int Void
IF_NOT_FAIL{ EXIT }
POP_FRAME
FAIL


bottles$1 Int Void:
...
LTE
IF_FALSE{FAIL}
...
CALL print
IF_FAIL{FAIL}
...
POP_FRAMEHEAD
EXIT
*/
