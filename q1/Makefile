CFLAGS = -Wall
LDFLAGS = -lm 
THREADS = -pthread
CC = gcc 
PROG = ex3_q1

all: test 

ex3_q1_given.o: ex3_q1_given.c ex3_q1_given.h
	$(CC)  $(CFLAGS) -c ex3_q1_given.c
		
$(PROG): $(PROG).c ex3_q1_given.o
	$(CC) $(CFLAGS) ex3_q1_given.o $(PROG).c  -o $(PROG) $(LDFLAGS) $(THREADS)

test: $(PROG) 
	./$(PROG)  > out.log 2> err.log

clean:
	rm -vf *.o  $(PROG) *.log