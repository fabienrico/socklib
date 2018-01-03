REP=$(shell pwd | sed "s%/.*/%%")

ABSREP=$(abspath .)
RELREP=.

NOMLIB=socklib

# définition de répertoire
REPSRCLIB=$(RELREP)/src/lib/
REPLIB=$(RELREP)/lib/
REPSRCBIN=$(RELREP)/src/example/
REPBIN=$(RELREP)/bin/
REPSRCTEST=$(RELREP)/src/test/
REPTEST=$(RELREP)/bin/
REPINCLUDE=$(RELREP)/include/

#definition des options
CC=gcc
CPP=g++
CFLAGS=-g -Wall
CPPFLAGS=-g -Wall -std=c++11 -I. -I$(REPINCLUDE)
LDFLAGS=
LDDFLAGS=-L$(REPLIB) -l$(NOMLIB) -Wl,-rpath,$(REPLIB)
# 
#-Wl,-rpath,$(REPLIB) est une option pour que l'executable retrouve la librairie dynamique.
#-Wl transmet la suite comme option à l'éditeur de lien.
#pour ld l'option -rpath ./lib/ lui fait ajouter ./lib au chemin de recherche des librairies

#variables auto générées
SRCLIB=$(wildcard $(addprefix $(REPSRCLIB)/,*.cpp))
POLIB=$(SRCLIB:.cpp=.o)
PHLIB=$(wildcard $REPINCLUDE/*.hpp)
FICHIERSLIB=$(REPLIB)lib$(NOMLIB).a $(REPLIB)lib$(NOMLIB).so
SRCTEST=$(wildcard $(addprefix $(REPSRCTEST),*.cpp))
SRCBIN=$(shell ls $(REPSRCBIN)/*.cpp)

EXECTEST=$(addprefix $(REPBIN)/,$(notdir $(SRCTEST:.cpp=.exx)))
EXECBIN=$(addprefix $(REPBIN)/,$(notdir $(SRCBIN:.cpp=.exx)))

REPSRC=$(REPSRCLIB) $(REPSRCBIN) $(REPSRCTEST)
FICHIERSGENERES=$(POLIB) $(FICHIERSLIB) $(EXECTEST) $(EXECBIN) Doxyfile
REPSGENERES=docs

bin: $(EXECBIN)

lib: $(FICHIERSLIB)

test: $(EXECTEST)
	@for i in $(EXECTEST); \
        do \
#		export LD_LIBRARY_PATH=$(REPLIB);\
                if [ -f $$i ]; then \
                        echo "###################################";\
                        echo "###################################";\
			echo "###                             ###";\
			echo "###    lance $$i  ###";\
			echo "###                             ###";\
                        echo "###################################";\
                        echo "###################################";\
			LD_LIBRARY_PATH=$(REPLIB) $$i;\
#			echo "nomexec="$$i;\
#			echo "retour $$?"; \
			if [ $$? -ne 0 ]; then \
				exit $?; \
			fi; \
                        echo "###################################";\
                        echo "###################################";\
			echo "###                             ###";\
			echo "###    test réussit             ###";\
			echo "###                             ###";\
                        echo "###################################";\
                        echo "###################################";\
                fi; \
        done
	echo $(EXECTEST)


%.o: %.cpp $(PHLIB)
	$(CPP) -c $(CPPFLAGS)  $<  -o $@

$(REPSRCLIB)/%.o: $(REPSRCLIB)/%.cpp $(PHLIB)
	$(CPP) -c -fPIC $(CPPFLAGS) -I $(REPINCLUDE) $<  -o $@


$(REPBIN)/%.exx:  $(REPSRCTEST)/%.cpp $(FICHIERSLIB)
	$(CPP) $(CPPFLAGS) $< $(LDDFLAGS) -o $@

$(REPBIN)/%.exx:  $(REPSRCBIN)/%.cpp $(FICHIERSLIB)
	$(CPP) $(CPPFLAGS) $< $(LDDFLAGS) -o $@


%.a: $(POLIB)
	ar cr $@ $(POLIB)

%.so: $(POLIB)
	$(CPP) -shared -fPIC -o $@ $(POLIB)



clean:
	@for i in $(FICHIERSGENERES); \
        do \
                if [ -f $$i ]; then \
                        echo "efface "$$i;\
                        rm $$i; \
                fi; \
        done
	@for i in $(REPSGENERES); \
        do \
                if [ -d $$i ]; then \
                        echo "déplace "$$i" dans /tmp";\
			NAM=`basename $$i;` \
			TMPNAM=`mktemp -d "/tmp/"$$NAM".XXXXXX"`;\
                        mv $$i $$TMPNAM;\
                fi; \
        done


tgz:: clean
	tar czvf ../$(REP).tar.gz -C ..  --exclude '*.swp' --exclude '*~' --exclude '.svn' --exclude 'docs' --exclude 'bin/*' --exclude 'lib/*' --exclude '.git'  $(REP)

tgz-date:: clean
	tar czvf ../$(REP)-`date +%y%m%d`.tar.gz -C ..  --exclude '*.swp' --exclude '*~' --exclude '.svn' --exclude 'docs' --exclude 'bin/*' --exclude 'lib/*' --exclude '.git'  $(REP)


Doxyfile : Makefile
	doxygen -g
	perl -p -i -e "s/^PROJECT_NAME.*$$/PROJECT_NAME = SockLib/" Doxyfile
	perl -p -i -e "s/^PROJECT_BRIEF.*$$/PROJECT_BRIEF = \"Librairie d'application pour le cours de système en L2 informatique à l'UCBL\"/" Doxyfile
	perl -p -i -e "s/^USE_MDFILE_AS_MAINPAGE =.*$$/USE_MDFILE_AS_MAINPAGE = README.md/" Doxyfile
	perl -p -i -e "s%^INPUT\s*=.*$$%INPUT = $(REPSRC) $(REPINCLUDE) README.md%" Doxyfile
	perl -p -i -e "s/^OUTPUT_DIRECTORY\s*=\s*$$/OUTPUT_DIRECTORY = /" Doxyfile
	perl -p -i -e "s/^GENERATE_HTML\s*=.*$$/GENERATE_HTML = YES/" Doxyfile
	perl -p -i -e "s/^HTML_OUTPUT\s*=.*$$/HTML_OUTPUT = docs/" Doxyfile
	perl -p -i -e "s/^GENERATE_LATEX\s*=.*$$/GENERATE_LATEX = NO/" Doxyfile
	perl -p -i -e "s/^BUILTIN_STL_SUPPORT\s*=\s*NO$$/BUILTIN_STL_SUPPORT = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_ALL\s*=\s*NO$$/EXTRACT_ALL = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_PRIVATE\s*=\s*NO$$/EXTRACT_PRIVATE = YES/" Doxyfile
	perl -p -i -e "s/^EXTRACT_STATIC\s*=\s*NO$$/EXTRACT_STATIC = YES/" Doxyfile
	perl -p -i -e "s/^OUTPUT_LANGUAGE\s*=\s*English$$/OUTPUT_LANGUAGE = French/i" Doxyfile

doc : Doxyfile $(FICHIERS)
	doxygen

