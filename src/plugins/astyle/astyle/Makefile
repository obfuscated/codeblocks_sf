# "Artistic Style" Makefile

CPPFLAGS = -Wall -Wno-sign-compare -O2
OBJS = ASResource.o ASBeautifier.o ASFormatter.o astyle_main.o

astyle: $(OBJS)
	g++ $(CPPFLAGS) -o astyle  $(OBJS)

.cpp.o:
	g++ $(CPPFLAGS) -c $<
.SUFFIXES: .cpp .c .o

clean:
	rm *.o
