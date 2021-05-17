DEBUG	= -g
CC	= gcc -c
LD	= gcc
CFLAGS	= $(DEBUG) -W -Wall
LIBS	=
LDFLAGS = $(DEBUG)

OBJS = thread.o integral.o

all : thread

thread  : $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

thread.o : thread.c integral.h
	$(CC) $(CFLAGS) $< -o $@

integral.o : integral.c integral.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) thread
