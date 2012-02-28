
all: metalan_prime metalan_doppel

NANOM = nanom.h nanom_vm.h nanom_asm.h nanom_stringlib.h

vm_test: ${NANOM} vm_test.cc
	g++ -std=c++0x vm_test.cc -o vm_test -g

metalan_prime: ${NANOM} metalan.h metalan_prime.cc metalan_prime.h
	g++ -std=c++0x metalan_prime.cc -o metalan_prime -g

metalan_doppel: ${NANOM} metalan.h metalan_doppel.cc metalan_doppel.h
	g++ -std=c++0x metalan_doppel.cc -o metalan_doppel -g

