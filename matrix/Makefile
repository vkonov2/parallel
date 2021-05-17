NAME = thread_mult

DEBUG	= -g
CC	= gcc -c
LD	= gcc
CFLAGS	= $(DEBUG) -W -Wall
LIBS	= -lpthread -lm
LDFLAGS = $(DEBUG)

OBJS = main.o matrices.o synchronize.o get_time.o

all : $(NAME)

$(NAME)  : $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

main.o: main.c matrices.h get_time.h
	$(CC) $(CFLAGS) $< -o $@
matrices.o: matrices.c matrices.h synchronize.h
	$(CC) $(CFLAGS) $< -o $@
synchronize.o: synchronize.c synchronize.h
	$(CC) $(CFLAGS) $< -o $@
get_time.o: get_time.c get_time.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(NAME)
