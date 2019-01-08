CC = cc
CFLAGS = -std=c99 -Wall -Werror -Wfatal-errors -g
SOURCE = mytail.c
OBJECTS = ${SOURCE:.c=.o}

mycat:	${OBJECTS}
	${CC} -o mytail ${CFLAGS} ${OBJECTS}
	rm ${OBJECTS}

mycat.o: mytail.c
	${CC} -c ${CFLAGS} mytail.c

clean:
	rm -f *.o *.core mytail
