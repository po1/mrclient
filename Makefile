CC=gcc
CFLAGS=-O2
LIBS=-lpthread -lncursesw
LDFLAGS=

all: mrclient

objs := $(patsubst %.c,%.o,$(wildcard *.c))

mrclient: $(objs)
	$(CC) -o $@ $(LIBS) $(LDFLAGS) $^

