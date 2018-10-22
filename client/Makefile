BASEDIR	=	..

CC	=	g++
CXX	=	g++
CPPFLAGS = -Wall	-std=c++11	-I ${BASEDIR}/include
LDFLAGS = -L ${BASEDIR}/lib
LDLIBS =	-lsystem_tools	-lsocket4L

all:	client

clean:
	rm -f client

build:
	cd	${BASEDIR}/src/tools	&&	${MAKE}	install
	cd	${BASEDIR}/src/socket	&&	${MAKE}	install
