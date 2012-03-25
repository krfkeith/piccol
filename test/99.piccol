

def [a:Int b:Sym c:Sym];

def {n:Int do_caps:Bool} TextNum;

def [a:TextNum b:Sym c:Sym];

def [a:Int b:Int c:Int];

print TextNum->Void :- 
  <: \n == 0 :> ? \do_caps ->Sym (\v ? 'No more'; 'no more') print ;
  \n print.

opt_s Int->Void :-
  <: \v != 1 :> ? 's' print; .

opt_s TextNum->Void :-
  \n opt_s.

print [Int Sym Sym]->Void :-
  \a print \b print \a opt_s \c print.

print [TextNum Sym Sym]->Void :-
  \a print \b print \a opt_s \c print.

bottles Int->Void :-
  <: \v == 0 :> ? 
     TextNum{n=\v do_caps=true} print 
     ' bottles of beer on the wall, ' print
     TextNum{n=\v do_caps=false} print 
     ' bottles of beer.\n' print 
     'Go to the store and buy some more, 99 bottles of beer on the wall.\n' print
;
  [\v ' bottle' ' of beer on the wall, '] print
  [\v ' bottle' ' of beer,\n'] print
  'Take one down, pass it around, ' print
  <: \v - 1 :> ->Void ( 
      [ TextNum{n=\v do_caps=false} ' bottle' ' of beer on the wall.\n\n' ] print 
      \v bottles
      ).

bottles Void->Void :-
  99 bottles
.

