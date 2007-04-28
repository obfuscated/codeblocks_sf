# Makefile for sheetdemo example for the wxsheet lib (UNIX).

# ----------------------------------------------------------------------------
# wxWidgets build settings

WXCONFIG := wx-config

WXDIR      = $(shell $(WXCONFIG) --prefix)
WXEXECDIR  = $(shell $(WXCONFIG) --exec-prefix)
WXLIBDIR   = $(WXEXECDIR)/lib

WXBASENAME = $(shell $(WXCONFIG) --basename)
WXVERSION  = $(shell $(WXCONFIG) --version)
WXRELEASE  = $(shell $(WXCONFIG) --release)

WXCXX      = $(shell $(WXCONFIG) --cxx)
WXCXXFLAGS = $(shell $(WXCONFIG) --cxxflags)
WXLIBS     = $(shell $(WXCONFIG) --libs)

WXSTC_LIB  = $(WXBASENAME)_stc-$(WXRELEASE)

# ----------------------------------------------------------------------------

WXTHINGS_LIBNAME = $(WXBASENAME)_wxthings-$(WXRELEASE)
WXTHINGS_LIBDIR = $(WXLIBDIR)

LIBVERSION_CURRENT=1
LIBVERSION_REVISION=0
LIBVERSION_AGE=0

TARGET_LIBNAME = lib$(WXTHINGS_LIBNAME)

TARGET_LIB_STATIC = $(WXTHINGS_LIBDIR)/$(TARGET_LIBNAME).a
TARGET_LIB_SHARED = $(WXTHINGS_LIBDIR)/$(TARGET_LIBNAME).so.$(LIBVERSION_CURRENT).$(LIBVERSION_REVISION).$(LIBVERSION_AGE)
TARGET_LIB_LINK1  = $(TARGET_LIBNAME).so.$(LIBVERSION_CURRENT)
TARGET_LIB_LINK2  = $(TARGET_LIBNAME).so

# ----------------------------------------------------------------------------

WXSTEDIT_LIBDIR = ../../../wxstedit
WXSTEDIT_LIBNAME = $(WXBASENAME)_stedit-$(WXRELEASE)

# ----------------------------------------------------------------------------

CXXFLAGS = $(WXCXXFLAGS) -MMD -g  -Wall -Wunused -Wunused-parameter
LDLIBS   = $(WXLIBS)
CXX      = $(WXCXX)

# -pg in libs and defs for profiling
PROFILE = #-pg
APPEXTRADEFS = -I$(WXDIR)/contrib/include -I../../include -I$(WXSTEDIT_LIBDIR)/include $(PROFILE)
APPEXTRALIBS =  -l$(WXSTC_LIB) $(PROFILE)

# ----------------------------------------------------------------------------

PROGRAM = wxfilebrowser

OBJECTS = wxfilebrowser.o

DEPFILES=$(OBJECTS:.o=.d)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(APPEXTRADEFS) -o $@ $<

all: $(WXSTEDIT_LIBNAME) $(WXTHINGS_LIBNAME) $(PROGRAM)

$(WXSTEDIT_LIBNAME) :
	@(cd $(WXSTEDIT_LIBDIR)/src && make)

$(WXTHINGS_LIBNAME) :
	@(cd ../../src && make)

# wxfilebrowser program using shared filebrws library
$(PROGRAM): $(PROGRAM).o $(TARGET_LIB_SHARED)
	$(CXX) $(LDFLAGS_EXE) -o $@ $(PROGRAM).o $(LDLIBS) $(APPEXTRALIBS) -l$(WXSTEDIT_LIBNAME) -l$(WXTHINGS_LIBNAME)

# wxfilebrowser program using static filebrws library
#$(PROGRAM): $(PROGRAM).o $(TARGET_LIB_STATIC)
#	$(CXX) $(LDFLAGS_EXE) -o $@ $(PROGRAM).o $(LDLIBS) $(APPEXTRALIBS) $(TARGET_LIB_STATIC)

# wxfilebrowser program using just object files and not the filebrws lib
#$(PROGRAM): $(PROGRAM).o $(OBJECTS)
#	$(CXX) $(LDFLAGS_EXE) -o $@ $(PROGRAM).o $(OBJECTS) $(LDLIBS) $(APPEXTRALIBS)

clean:
	rm -f $(OBJECTS) $(DEPFILES) $(PROGRAM) $(PROGRAM).o $(PROGRAM).d core

cleanall: clean
	@(cd ../../src && make clean)


-include $(DEPFILES)

