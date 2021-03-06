# Shawn brown
# makefile



CC      = gcc
CFLAGS  = -g #-Wall -Wshadow



EXEC1 := oss
SRC1 := oss.c sysclock.c
OBJ1 := $(patsubst %.c, %.o, $(SRC1))	

all: $(EXEC1)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(EXEC1)  -lm

EXEC2 := user_proc
SRC2 := sysclock.c user_proc.c
OBJ2 := $(patsubst %.c, %.o, $(SRC2))	

all: $(EXEC2)

$(EXEC2): $(OBJ2) 
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2) -lm


.PHONY: clean

clean:
	rm -f $(EXEC1) $(EXEC2) *.o logfile

