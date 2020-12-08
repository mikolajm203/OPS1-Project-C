CC=gcc
CFLAGS= -std=gnu99 -Wall
LDLIBS = -lpthread -lm
all: main
main:
	${CC} ${CFLAGS} -o main main.c map.c ${LDLIBS}
.PHONY:
	clean all
clean:
	rm main
