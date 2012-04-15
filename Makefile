
all: utils/metalan_prime utils/metalan_doppel utils/metalan_idem utils/macrolan utils/piccol_test utils/modulum_test

CFLAGS = -std=c++0x -O3 -g -Wall -I.

clean:
	-rm utils/metalan_prime utils/metalan_doppel utils/metalan_idem utils/macrolan utils/piccol_test utils/modulum_test

utils/metalan_prime: metalan.h utils/metalan_prime.cc metalan_prime.h
	g++ $(CFLAGS) utils/metalan_prime.cc -o utils/metalan_prime 

utils/metalan_doppel: metalan.h utils/metalan_doppel.cc metalan_doppel.h
	g++ $(CFLAGS) utils/metalan_doppel.cc -o utils/metalan_doppel 

utils/metalan_idem: utils/metalan_prime
	g++ $(CFLAGS) utils/metalan_idem.cc -o utils/metalan_idem

utils/macrolan: macrolan.h utils/metalan_prime
	g++ $(CFLAGS) utils/macrolan.cc -o utils/macrolan

SRC = metalan.h nanom.h piccol_vm.h piccol_asm.h metalan_prime.h metalan_doppel.h macrolan.h 

utils/piccol_test: $(SRC) utils/piccol_test.cc 
	g++ $(CFLAGS) utils/piccol_test.cc -o utils/piccol_test

utils/modulum_test: $(SRC) utils/modulum_test.cc piccol_modulum.h
	g++ $(CFLAGS) utils/modulum_test.cc -o utils/modulum_test

