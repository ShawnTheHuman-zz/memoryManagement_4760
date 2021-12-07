# Shawn brown
# makefile


CC      = g++
CFLAGS  = -std=c++11 -g -Wall -Wshadow
DEPS = sysclock.h
OBJ = oss.o user_proc.o sysclock.o
EXEC = oss user_proc

all: $(EXEC)


$(EXEC): %: %.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): %.o: %.cpp $(DEPS)
	$(CC)  $(CLAGS) -o $@ -c $<


.PHONY: clean

clean:
	rm -f $(EXEC) *.o logfile

