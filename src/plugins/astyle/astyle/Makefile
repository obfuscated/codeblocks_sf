# "Artistic Style" Makefile

CPPBASEFLAGS = -Wall -W -fno-rtti -fno-exceptions
OBJ =  astyle_main.o  ASBeautifier.o  ASFormatter.o  ASEnhancer.o  ASResource.o 
BIN = astyle

#check for debug info
ifdef DEBUG
CPPFLAGS = -DDEBUG -g $(CPPBASEFLAGS)
LDFLAGS =
else
CPPFLAGS = -DNDEBUG -O3 $(CPPBASEFLAGS)
LDFLAGS = -s
endif

astyle: $(OBJ)
	g++ $(LDFLAGS) -o $(BIN) $(OBJ)

.cpp.o:
	g++ $(CPPFLAGS) -c $<

# Targets that are not files
.PHONY: 
	all clean

all:
	$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
