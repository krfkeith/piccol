

def [a:Int b:Sym c:Sym];

def {n:Int do_caps:Bool} TextNum;

def [a:TextNum b:Sym c:Sym];

def [a:Int b:Int c:Int];

print TextNum->Void :- 
  [\n 0] $eq ? \do_caps ->Sym (\v ? 'No more'; 'no more') print;
  \n print.

opt_s Int->Void :-
  [\v 1] $eq $not ? 's' print; .

opt_s TextNum->Void :-
  \n opt_s.

print [Int Sym Sym]->Void :-
  \a print \b print \a opt_s \c print.

print [TextNum Sym Sym]->Void :-
  \a print \b print \a opt_s \c print.

bottles Int->Void :-
  [ \v 0 ] $eq ? 
     TextNum{n=\v do_caps=true} print 
     ' bottles of beer on the wall, ' print
     TextNum{n=\v do_caps=false} print 
     ' bottles of beer.\n' print 
     'Go to the store and buy some more, 99 bottles of beer on the wall.\n' print
;
  [\v ' bottle' ' of beer on the wall, '] print
  [\v ' bottle' ' of beer,\n'] print
  'Take one down, pass it around, ' print
  [\v 1] $sub ->Void ( 
      [ TextNum{n=\v do_caps=false} ' bottle' ' of beer on the wall.\n\n' ] print
      \v bottles
      ).

bottles Void->Void :-
  [([(0)(1)] $add)(1)] $add print
  99 bottles
  [1 2 3] ->Int (

[([([([([([([([(\b $neg)] $noop)([([([([([([([([([([(\b)([(\b)] $noop)] $mul)] $noop)] $noop)] $noop)] $noop)([([(4)([(\a)([(\c)] $noop)] $mul)] $mul)] $noop)] $sub)] $noop)] $noop)] $noop)] $noop)] $sub)] $noop)] $noop)([([([([([(2)([(\a)] $noop)] $mul)] $noop)] $noop)] $noop)] $noop)] $div)] $noop)] $noop)] $noop 

  ) print.




/*

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

0
.a
EQ
IF_FALSE{FALSE}
CALL_LIGHT $2

*/
