/*
 * -= Copyright 2005 Tim Baker (treectrl@hotmail.com) =-
 *
 * This file is part of depslib.
 *
 * License is hereby granted to use this software and distribute it
 * freely, as long as this copyright notice is retained and modifications 
 * are clearly marked.
 *
 * ALL WARRANTIES ARE HEREBY DISCLAIMED.
 */
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if !defined(DEPSLIB_WINDOWS) && !defined(DEPSLIB_UNIX)
#error "must define one of DEPSLIB_WINDOWS or DEPSLIB_UNIX"
#endif

#ifdef DEPSLIB_WINDOWS
#define USE_PATHUNIX
#define PATH_DELIM '\\'
#define OS_NT
#define DOWNSHIFT_PATHS
#endif

#ifdef DEPSLIB_UNIX
#define USE_FILEUNIX
#define USE_PATHUNIX
#define PATH_DELIM '/'
#endif

#define DEBUG_BINDSCAN 0
#define DEBUG_HEADER 0
#define DEBUG_LISTS 0
#define DEBUG_MEM 0
#define DEBUG_SEARCH 0

#define MAXJPATH 1024
#define MAXSYM 1024

#define USE_CACHE

extern const char *search(const char *source, const char *_header, time_t *time);

#ifdef DBALLOC
extern void *_db_malloc(size_t siz, char *file, int line);
extern void *_db_realloc(void *ptr, size_t siz, char *file, int line);
extern void _db_free(void *ptr, char *file, int line);
#define malloc(siz) _db_malloc(siz,__FILE__,__LINE__)
#define realloc(str,siz) _db_realloc(str,siz,__FILE__,__LINE__)
#define free(str) _db_free(str,__FILE__,__LINE__)
#endif
