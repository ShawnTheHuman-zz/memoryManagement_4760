# Shawn brown
# makefile



CC      = gcc
CFLAGS  = -g -Wall -Wshadow
DEPS = sysclock.h user_proc.cpp oss.cpp sysclock.cpp bitvector.cpp bitvector.h oss.h
OBJ = oss.o user_proc.o sysclock.o


EXEC1 := oss
SRC1 := sysclock.cpp bitvector.cpp oss.cpp
OBJ1 := $(patsubst %.cpp, %.o, $(SRC1))	

all: $(EXEC1)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJ1)

EXEC2 := user_proc
SRC2 := sysclock.cpp bitvector.cpp user_proc.cpp
OBJ2 := $(patsubst %.cpp, %.o, $(SRC2))	

all: $(EXEC2)

$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2)


.PHONY: clean

clean:
	rm -f $(EXEC1) $(EXEC2) *.o logfile

