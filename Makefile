
all: vm_test metalan_test

vm_test: nanom.h nanom_stringlib.h vm_test.cc
	g++ -std=c++0x vm_test.cc -o vm_test -g

metalan_test: metalan.h metalan_test.cc metalan_prime.h
	g++ -std=c++0x metalan_test.cc -o metalan_test -g

