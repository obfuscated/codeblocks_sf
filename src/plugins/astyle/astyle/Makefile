# $Id$
# --------------------------------------------------------------------------
#
# Copyright (C) 1998,1999,2000,2001,2002 Tal Davidson.
# Copyright (C) 2004 Martin Baute.
# All rights reserved.
#
# This file is a part of "Artistic Style" - an indentation and reformatting
# tool for C, C++, C# and Java source files - http://astyle.sourceforge.net
#
# --------------------------------------------------------------------------
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# --------------------------------------------------------------------------

# Debug
CXXFLAGS = -DNDEBUG

# Release
# CFLAGS = -DNDEBUG

.PHONY: all release debug lib clean

all: astyle

release: astyle

debug:
	$(MAKE) CXXFLAGS=-g

lib: libastyle.a

clean:
	-@rm *.o
	-@rm *.a

ASFormatter.o: ASFormatter.cpp
	$(CXX) $(CXXFLAGS) -Wall -O2 -c ASFormatter.cpp  -o ASFormatter.o

ASBeautifier.o: ASBeautifier.cpp
	$(CXX) $(CXXFLAGS) -Wall -O2 -c ASBeautifier.cpp -o ASBeautifier.o

astyle_main.o: astyle_main.cpp
	$(CXX) $(CXXFLAGS) -Wall -O2 -c astyle_main.cpp  -o astyle_main.o

libastyle.a: ASBeautifier.o ASFormatter.o
	$(AR) r libastyle.a ASBeautifier.o ASFormatter.o

astyle: libastyle.a astyle_main.o
	$(CXX) $(CXXFLAGS) -o astyle astyle_main.o libastyle.a

