CFLAGS = -Wall -Werror 

all: build

build:
	gcc $(CFLAGS) s-talk.c monRoutines.h monRoutines.c  list.h list.c -o s-talk -lpthread
	
clean:
	rm -f s-talkss