
all: vm_test metalan_test

NANOM = nanom.h nanom_vm.h nanom_asm.h nanom_stringlib.h

vm_test: ${NANOM} vm_test.cc
	g++ -std=c++0x vm_test.cc -o vm_test -g

metalan_test: ${NANOM} metalan.h metalan_test.cc metalan_prime.h
	g++ -std=c++0x metalan_test.cc -o metalan_test -g

