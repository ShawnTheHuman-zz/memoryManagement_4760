# Shawn brown
# makefile



CC      = gcc
CFLAGS  = -g -Wall -Wshadow
DEPS = sysclock.h user_proc.c oss.c sysclock.c oss.h
OBJ = oss.o user_proc.o sysclock.o


EXEC1 := oss
SRC1 := sysclock.c oss.c
OBJ1 := oss.o sysclock.h	

all: $(EXEC1)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXEC1) oss.o -lm

EXEC2 := user_proc
SRC2 := sysclock.c user_proc.cpp
OBJ2 := user_proc.o sysclock.h

all: $(EXEC2)

$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) user_proc.o -lm


.PHONY: clean

clean:
	rm -f $(EXEC1) $(EXEC2) *.o logfile

