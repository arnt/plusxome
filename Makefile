CPP = clang
CFLAGS += -O0 -I. -W -Wall -g -Wno-c++11-extensions -Wno-local-type-template-args
OPTIM =
LD = clang
LFLAGS = -g ;
LIBS = -lstdc++ -lboost_thread -lboost_filesystem -lboost_system -lboost_program_options -ltidy

OBJECTS=plusxome.o \
	rendering.o \
	template.o document.o node.o \
	path.o \
	post.o asset.o file.o \
	httplistener.o httpserver.o \
	plugin.o lastresort.o \
	filewatcher.o \
	plugins/singlepost.o

all: plusxome

.cpp.o: Makefile
	${CPP} -g -c -o $@ ${CFLAGS} $<

plusxome: ${OBJECTS} Makefile
	${CPP} -g -o plusxome -pthread ${OBJECTS} ${LIBS}

clean:
	-rm plusxome *.o plugins/*.o
