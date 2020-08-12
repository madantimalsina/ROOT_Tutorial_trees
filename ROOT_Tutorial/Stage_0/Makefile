CPPFLAGS=-g -O2 -Wall -m64 -std=gnu++11
LIBS=  -g -m64

CC:=g++

.PHONY: all clean clear

all: testCEST



testCEST: testCEST.o
	$(CC) $^ -o $@ $(LIBS)
testCEST.o : main.cxx
	$(CC) -c $(CPPFLAGS) $< -o $@
clean:clear
clear:
	rm -rf testCEST
	rm -rf ./*.o


