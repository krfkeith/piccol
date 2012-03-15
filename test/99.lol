
/*
<<

intexpr :- \any:'Int' '+' \any:'Int' @''

>>

*/

def [ a:Int b:Int ];
def [ a:Real b:Real ];

def [ a:Int b:Sym ];

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


bottles Int->Void :-
