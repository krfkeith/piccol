
all: metalan_prime metalan_doppel metalan_idem macrolan piccol_test

CFLAGS = -std=c++0x -g -Wall

metalan_prime: metalan.h metalan_prime.cc metalan_prime.h
	g++ $(CFLAGS) metalan_prime.cc -o metalan_prime 

metalan_doppel: metalan.h metalan_doppel.cc metalan_doppel.h
	g++ $(CFLAGS) metalan_doppel.cc -o metalan_doppel 

metalan_idem: metalan_prime
	g++ $(CFLAGS) metalan_idem.cc -o metalan_idem

macrolan: macrolan.h metalan_prime
	g++ $(CFLAGS) macrolan.cc -o macrolan

piccol_test: metalan.h nanom.h piccol_vm.h piccol_asm.h metalan_prime.h metalan_doppel.h macrolan.h piccol_test.cc 
	g++ $(CFLAGS) piccol_test.cc -o piccol_test



