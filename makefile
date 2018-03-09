# Makefile for CS 1783 Genetic Algorithm Assignment

a.out:		ga.o makefile
	g++ -g ga.o
ga.o:		ga.c datatypes.h
	g++ -g -c ga.c
