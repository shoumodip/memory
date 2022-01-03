CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=

memory: memory.c
	$(CC) $(CFLAGS) -o memory memory.c $(LIBS)
