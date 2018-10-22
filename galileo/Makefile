BASEDIR	=	..

CC	=	g++
CXX	=	g++
CPPFLAGS = -Wall -std=c++11	-I ${BASEDIR}/include
LDFLAGS = -L ${BASEDIR}/lib
LDLIBS =	-lsystem_tools	-lsocket4L	-lmraa

all:	main

main:	controller.h	controller.cpp

build:
	cd	${BASEDIR}/src/tools	&&	${MAKE}	install
	cd	${BASEDIR}/src/socket	&&	${MAKE}	install

clean:
	rm -f main	*.o
	cd	${BASEDIR}/src/tools	&&	${MAKE}	clean
	cd	${BASEDIR}/src/socket	&&	${MAKE}	clean
