SHELL := /bin/sh  
CC = clang  
CFLAGS = -Wall -Wextra -Werror -pedantic  
LIBFLAGS = -lm  
EXECBIN  = httpserver format  


SOURCES = $(wildcard *.c)

OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean

all: $(EXECBIN)
$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBFLAGS) asgn4_helper_funcs.a

clean:
	rm -f $(EXECBIN) $(OBJECTS) 

%.o : %.c
	$(CC) $(CFLAGS) -c $<


format:
	clang-format -i -style=file *.[ch]
