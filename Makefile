CPP = g++
C = gcc

SRCDIR		= ./
INCLUDEDIR = -I./
LIBRARIES = -L./ -pthread
CFLAGS = -g -c -Wall -O0 $(INCLUDEDIR) 


all:exe
	@echo "!!! build OK !!!"

objects = PodcastGrabber.o

PodcastGrabber.o: $(SRCDIR)PodcastGrabber.cpp $(SRCDIR)PodcastGrabber.h
	$(CPP) $< $(CFLAGS)


EXE_NAME=PodcastGrabber.exe
exe:$(EXE_NAME)
$(EXE_NAME):$(SRCDIR)test.cpp $(objects)
	$(CPP) -o $(EXE_NAME) $^ $(INCLUDEDIR) $(LIBRARIES)

clean:
	$(RM) -fr core *.o $(EXE_NAME)

install:

full: clean all install
