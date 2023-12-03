SHELL := /bin/sh  
CC = clang  
CFLAGS = -Wall -Wextra -Werror -pedantic  
LIBFLAGS = -lm  
EXECBIN  =  cacher format  


SOURCES = $(wildcard *.c)

OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean

all: $(EXECBIN)
$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBFLAGS)

clean:
	rm -f $(EXECBIN) $(OBJECTS) 

%.o : %.c
	$(CC) $(CFLAGS) -c $<


format:
	clang-format -i -style=file *.[ch]
