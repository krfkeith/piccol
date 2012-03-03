
all: metalan_prime metalan_doppel picolol_vm

NANOM = nanom.h nanom_vm.h nanom_asm.h nanom_stringlib.h

CFLAGS = -std=c++0x -g -Wall 

metalan_prime: metalan.h metalan_prime.cc metalan_prime.h
	g++ $(CFLAGS) metalan_prime.cc -o metalan_prime 

metalan_doppel: metalan.h metalan_doppel.cc metalan_doppel.h
	g++ $(CFLAGS) metalan_doppel.cc -o metalan_doppel 

picolol_vm: metalan.h nanom.h nanom_asm.h picolol_vm.cc picolol_vm.h metalan_prime.h metalan_doppel.h
	g++ $(CFLAGS) picolol_vm.cc -o picolol_vm 


