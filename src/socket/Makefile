BASEDIR	=	../..

CC	=	g++
CXX	=	g++
CPPFLAGS = -Wall	-I ${BASEDIR}/include
CFLAGS =
LDFLAGS = -L ${BASEDIR}/lib
LDLIBS =	-lsystem_tools

GENERATED	=	teste_server	teste_client	consumidor

all:	${GENERATED}	libsocket4L.a

${GENERATED}:	libsocket4L.a

libsocket4L.a:	socket4L.o
	ar r $@ $?

socket4L.o:	socket4L.cpp	socket4L.h

install:	libsocket4L.a
	cp -f libsocket4L.a ${BASEDIR}/lib
	cp -f socket4L.h ${BASEDIR}/include

clean:
	rm -f ${GENERATED}	*.a	*.o
