
all: metalan_prime metalan_doppel picolol_vm

NANOM = nanom.h nanom_vm.h nanom_asm.h nanom_stringlib.h

metalan_prime: metalan.h metalan_prime.cc metalan_prime.h
	g++ -std=c++0x metalan_prime.cc -o metalan_prime -g

metalan_doppel: metalan.h metalan_doppel.cc metalan_doppel.h
	g++ -std=c++0x metalan_doppel.cc -o metalan_doppel -g

picolol_vm: metalan.h atomsys.h atomsys_asm.h picolol_vm.cc picolol_vm.h
	g++ -std=c++0x picolol_vm.cc -o picolol_vm -g


