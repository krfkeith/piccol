
all: metalan_prime metalan_doppel picol_vm

NANOM = nanom.h nanom_vm.h nanom_asm.h nanom_stringlib.h

CFLAGS = -std=c++0x -g -Wall 

metalan_prime: metalan.h metalan_prime.cc metalan_prime.h
	g++ $(CFLAGS) metalan_prime.cc -o metalan_prime 

metalan_doppel: metalan.h metalan_doppel.cc metalan_doppel.h
	g++ $(CFLAGS) metalan_doppel.cc -o metalan_doppel 

picol_vm: metalan.h nanom.h nanom_asm.h picol_vm.cc picol_vm.h metalan_prime.h metalan_doppel.h
	g++ $(CFLAGS) picol_vm.cc -o picol_vm 


