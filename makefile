# Shawn brown
# makefile



CC      = gcc
CFLAGS  = -g -Wall -Wshadow
DEPS = sysclock.h user_proc.c oss.c sysclock.c oss.h
OBJ = oss.o user_proc.o sysclock.o


EXEC1 := oss
SRC1 := sysclock.c oss.c
OBJ1 := $(patsubst %.c, %.o, $(SRC1))	

all: $(EXEC1)

$(EXEC1): $(OBJ1) sysclock.h
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJ1) -lm

EXEC2 := user_proc
SRC2 := sysclock.c user_proc.cpp
OBJ2 := $(patsubst %.c, %.o, $(SRC2))	-lm

all: $(EXEC2)

$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2)


.PHONY: clean

clean:
	rm -f $(EXEC1) $(EXEC2) *.o logfile

