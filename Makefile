
#########################################################
# Makefile for Genetic Algorithm Application            #
#########################################################

CFLAGS += -std=c99 -Wall -Wextra -Ilib -fopenmp -o2
LDFLAGS += -lm

#########################################################
# Make All

all:  ga

#########################################################
# Make Genetic Algorithm (Parallel)

ga:  ga.c datatypes.h
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

#########################################################
# Make Genetic Algorithm gaS (Serial Baseline)

gaS:  gaS.c datatypes.h
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

##########################################################
# Make Clean

clean:
	$(RM) ga gaS

.PHONY: clean

#########################################################



# END Makefile for Mandelbrot Set Application  - EWG SDG

