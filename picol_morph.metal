

other :- * &''.
two :- * other &''.

swap :- '(' @'(' two ')' @')'.

all :- swap all.
all :- *.
all :- .

main :- all.

