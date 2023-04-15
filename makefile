# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -std=c++17
 
# ****************************************************
# Targets needed to bring the executable up to date

Compile: main.o Parser.o McFunction.o Names.o McInt_scoreoard.o Lekser/lekser.h McRun_function.o McInt_array.o OutsideFunction.o
	$(CC) $(CFLAGS) -o Compile main.o Parser.o McFunction.o Names.o McInt_scoreoard.o McRun_function.o McInt_array.o OutsideFunction.o
main.o: main.cpp Parser/Parser.h Lekser/lekser.h
	$(CC) $(CFLAGS) -c main.cpp
Parser.o: Parser/Parser.cpp Parser/Parser.h Parser/McFunction.h
	$(CC) $(CFLAGS) -c Parser/Parser.cpp
McFunction.o: Parser/McFunction.cpp Parser/McFunction.h
	$(CC) $(CFLAGS) -c Parser/McFunction.cpp
OutsideFunction.o: Parser/OutsideFunction.cpp Parser/OutsideFunction.h
	$(CC) $(CFLAGS) -c Parser/OutsideFunction.cpp
McRun_function.o: Parser/McRun_function.cpp Parser/McFunction.h
	$(CC) $(CFLAGS) -c Parser/McRun_function.cpp
McInt_scoreoard.o: Parser/McInt_scoreoard.cpp Parser/McFunction.h
	$(CC) $(CFLAGS) -c Parser/McInt_scoreoard.cpp
McInt_array.o: Parser/McInt_array.cpp Parser/McFunction.h
	$(CC) $(CFLAGS) -c Parser/McInt_array.cpp
Names.o: Names.cpp
	$(CC) $(CFLAGS) -c Names.cpp
clean: 
	rm -f McFunction.o McInt_scoreoard.o Parser.o main.o McRun_function.o McInt_array.o OutsideFunction.o Names.o
