# Make file for GCC compiler on Linux or compatible OS
# The license.html file describes the conditions under which this software may be distributed.

# list of source files
SRC = astyle_main.cpp \
      ASBeautifier.cpp \
      ASFormatter.cpp \
      ASEnhancer.cpp \
      ASResource.cpp

# source directories
vpath %.cpp ../src
vpath %.h   ../src

# install directory $(ipath)
# set prefix if not defined on the command line
ifndef prefix
    prefix=/usr/bin
endif
ipath=$(prefix)

# define macros
CBASEFLAGS = -W -Wall -fno-rtti -fno-exceptions
INSTALL=install -o $(USER) -g $(USER)

##################################################

# define compile options for each build
CFLAGS   = -O3 $(CBASEFLAGS)
CFLAGSd  = -g $(CBASEFLAGS)
CFLAGSs  = -DASTYLE_LIB -O3 -fpic $(CBASEFLAGS)
CFLAGSsd = -DASTYLE_LIB -g -fpic $(CBASEFLAGS)
CFLAGSa  = -DASTYLE_LIB -O3 $(CBASEFLAGS)
CFLAGSad = -DASTYLE_LIB -g $(CBASEFLAGS)

# object files are built from the source list $(SRC)
# a suffix is added for each build
OBJ   = $(patsubst %.cpp,%.o,$(SRC))
OBJd  = $(patsubst %.cpp,%_d.o,$(SRC))
OBJs  = $(patsubst %.cpp,%_s.o,$(SRC))
OBJsd = $(patsubst %.cpp,%_sd.o,$(SRC))
OBJa  = $(patsubst %.cpp,%_a.o,$(SRC))
OBJad = $(patsubst %.cpp,%_ad.o,$(SRC))

# define object file rule (with the suffix) for each build

# OBJ
%.o:  %.cpp  astyle.h
	g++ $(CFLAGS) -c -o $@ $<

# OBJd
%_d.o:  %.cpp  astyle.h
	g++ $(CFLAGSd) -c -o $@ $<

# OBJs
%_s.o:  %.cpp  astyle.h
	g++ $(CFLAGSs) -c -o $@ $<

# OBJsd
%_sd.o:  %.cpp  astyle.h
	g++ $(CFLAGSsd) -c -o $@ $<

# OBJa
%_a.o:  %.cpp  astyle.h
	g++ $(CFLAGSa) -c -o $@ $<

# OBJad
%_ad.o:  %.cpp  astyle.h
	g++ $(CFLAGSad) -c -o $@ $<

##################################################
# define build dependencies for each command

release:  astyle
astyle:  $(OBJ)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	g++ -o ../bin/$@ $^
	strip ../bin/$@

debug:  astyled
astyled:  $(OBJd)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	g++ -o ../bin/$@ $^

shared:  libastyle.so
libastyle.so:  $(OBJs)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	g++ -shared -o ../bin/$@ $^
	strip ../bin/$@

shareddebug:  libastyled.so
libastyled.so:  $(OBJsd)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	g++ -shared -o ../bin/$@ $^

static:  libastyle.a
libastyle.a:  $(OBJa)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	ar crs ../bin/$@ $^

staticdebug:  libastyled.a
libastyled.a:  $(OBJad)
	@ if [ ! -d ../bin ]; then mkdir ../bin; fi
	ar crs ../bin/$@ $^

all:  release debug shared shareddebug static staticdebug

clean:
	rm -f *.o

install:
	@ $(INSTALL) -m 755 -d $(ipath)
	$(INSTALL) -m 755 ../bin/astyle  $(ipath)

uninstall:
	rm -f $(ipath)/astyle
