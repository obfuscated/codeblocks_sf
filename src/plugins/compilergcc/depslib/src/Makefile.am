AM_CFLAGS = -Wall -g -DDEPSLIB_UNIX -fno-strict-aliasing

noinst_LTLIBRARIES = libdepslib.la

libdepslib_la_SOURCES = alloc.c \
			cache.c \
			depslib.c \
			filent.c \
			fileunix.c \
			hash.c \
			headers.c \
			lists.c \
			newstr.c \
			pathsplit.c \
			pathunix.c \
			regexp.c \
			search.c \
			timestamp.c

noinst_HEADERS = alloc.h \
			cache.h \
			depslib.h \
			filesys.h \
			hash.h \
			headers.h \
			jam.h \
			lists.h \
			newstr.h \
			pathsplit.h \
			pathsys.h \
			regexp.h \
			search.h \
			timestamp.h

EXTRA_INST = main.c
