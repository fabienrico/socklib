REP=$(shell pwd | sed "s%/.*/%%")

CODEC=
CODECPP=test_socklib.cpp demo_lecture.cpp
POINTH=$(wildcard *.h)
POINTHPP=$(wildcard *.hpp)
LIBCPP=socklib.cpp readerwriter.cpp bufferedreaderwriter.cpp

FICHIERSBAK=$(wildcard *~)


EXE=$(CODEC:.c=.ex) $(CODECPP:.cpp=.exx)
LIBCPPO=$(LIBCPP:.cpp=.O)


CC=gcc
CPP=g++
CFLAGS=-g -Wall
CPPFLAGS=-g -Wall -std=c++11 -I.
LDFLAGS=
LDDFLAGS=


FICHIERS=$(CODEC) $(POINTH) $(CODECPP) $(POINTHPP) $(LIBCPP)

all: $(EXE)

exec: $(EXE)

%.ex: %.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

%.exx: %.cpp $(LIBCPPO)
	$(CPP) $(CPPFLAGS) $< $(LIBCPPO) $(LDDFLAGS) -o $@

%.O: %.cpp $(POINTHPP)
	$(CPP) -c $(CPPFLAGS) $<  -o $@


%.class: %.java
	javac $<

clean:
	@for i in $(EXE) $(LIBCO) $(LIBCPPO); \
        do \
                if [ -f $$i ]; then \
                        echo "efface "$$i;\
                        rm $$i; \
                fi; \
        done


grosclean: clean
	@for i in $(FICHIERSBAK); \
        do \
                if [ -f $$i ]; then \
                        echo "efface "$$i;\
                        rm $$i; \
                fi; \
        done \

tgz:: clean
	tar czvf ../$(REP).tar.gz -C ..  --exclude '*.swp' --exclude '*~' --exclude '.svn' --exclude 'Docs' $(REP)

tgz-date:: clean
	tar czvf ../$(REP)-`date +%y%m%d`.tar.gz -C ..  --exclude '*.swp' --exclude '*~' --exclude '.svn' --exclude 'Docs' $(REP)

Doxyfile :
	doxygen -g
	perl -p -i -e "s/^OUTPUT_DIRECTORY\s*=\s*$$/OUTPUT_DIRECTORY = Docs/" Doxyfile
	perl -p -i -e "s/^BUILTIN_STL_SUPPORT\s*=\s*NO$$/BUILTIN_STL_SUPPORT = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_ALL\s*=\s*NO$$/EXTRACT_ALL = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_PRIVATE\s*=\s*NO$$/EXTRACT_PRIVATE = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_STATIC\s*=\s*NO$$/EXTRACT_STATIC = YES/" Doxyfile
	perl -p -i.bak -e "s/^OUTPUT_LANGUAGE\s*=\s*English$$/OUTPUT_LANGUAGE = French/i" Doxyfile

doc : Doxyfile $(FICHIERS)
	mkdir -p Docs
	doxygen
	rm Doxyfile
