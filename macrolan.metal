
space :- ' '.
space :- '\n'.
space :- '\t'.

spaces :- space spaces.
spaces :- .

ident_char :- \locase.
ident_char :- \upcase.
ident_char :- \digit.
ident_char :- '_'.

ident_x :- ident_char ident_x.
ident_x :- ident_char.
ident_x :-.
ident :- ident_char ident_x.

ident_here :- ident &''.

push_empty :- &'push'.


ident_list :- spaces ident_here spaces ',' ident_list.
ident_list :- spaces ident_here spaces.

macroname :- ident_here spaces '(' ident_list ')'.
macroname :- ident_here spaces.

macrodef_data :- '::>' &'pop' push_empty.
macrodef_data :- \any &'append' macrodef_data.

macroapp_data :- ':>' &'pop' push_empty.
macroapp_data :- \any &'append' macroapp_data.

macro :- @'DEFINE' ':' spaces macroname push_empty @':' macrodef_data.
macro :- @'APPLY' '[' ident_here ']' push_empty macroapp_data.
macro :- @'APPLY' @'default' push_empty macroapp_data.

all :- &'pop' '<:' macro @'TEXT' push_empty all.
all :- \any &'append' all.
all :- &'pop'.

main :- @'TEXT' &'push' all.
