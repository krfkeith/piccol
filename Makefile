
all: nanom.h nanom_stringlib.h vm_test.cc
	g++ -std=c++0x vm_test.cc -o vm_test
