# ======================================================================================
# Author: Francesco Montorsi
# RCS-ID: $Id$
#
# A basic "configure.ac" for a wxCode component.
# See 
#    http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_mono/autoconf.html
# for more info about autoconf and the autoconf macros.
# ======================================================================================


# Init this configure script with basic info about the component
# (DON'T PUT ANYTHING BEFORE AC_INIT, JUST COMMENTS)
AC_INIT([wxXml2], [1.5], [frm@sourceforge.net])

# ENABLES/DISABLES THE DEBUG MODE FOR THIS CONFIGURE SCRIPT
#DBG_CONFIGURE=1

m4_include(../../../build/autoconf/wxcode.m4)
AM_WXCODE_INIT




## CONFIGURE OPTIONS
##
## Before starting checks, declare the options of this configure script
## Here you should use the AC_ARG_ENABLE and AC_ARG_WITH macros
## to add to this configure script the --enable-XXX and/or --with-XXX
## options required. If you did not use any <option> tag in your
## component's bakefile, then you can leave this section as is
#####################################################################

AM_OPTIONS_WXCONFIG
AM_OPTIONS_WXPRESETS





## CONFIGURE CHECKS
##
## Here you should use the AC_CHECK_LIB, AC_COMPILE_IFELSE, 
## AC_LINK_IFELSE, etc macros to check that the libraries required
## by your component exist on the host machine and match your
## required options (version, build settings, etc)
#####################################################################

AM_WXCODE_CHECKS([2.4.0])       # 2.4.0 is the minimum required version of wx


# check for the libxml2 library
# AM_PATH_XML([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
# Test for XML, and define XML_CPPFLAGS and XML_LIBS
AM_PATH_XML2([2.6.1], [],
	[AC_MSG_ERROR([Cannot found the libxml2 library or your libxml2 is too old (libxml2 > 2.6.1 is required); download it from http://xmlsoft.org])])

# add all XML stuff that we've found...
CXXFLAGS="$CXXFLAGS $XML_CPPFLAGS"
CFLAGS="$CFLAGS $XML_CPPFLAGS"
LDFLAGS="$LDFLAGS $WX_LIBS $XML_LIBS"



# end in a nice way the configure script
AM_WXCODE_END
