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

#ifdef DEPSLIB_WINDOWS
#define SEP1 '\\'
#define SEP2 '/'
#endif

#ifdef DEPSLIB_UNIX
#define SEP1 '/'
#define SEP2 '/'
#endif

typedef struct _pathsplit PATHSPLIT;

struct _pathsplit
{
	PATHPART part[64];
	int count;
};

extern void path_split(const char *path, PATHSPLIT *f);
extern int path_normalize(PATHSPLIT *f, PATHSPLIT *cwd);
extern char *path_tostring(PATHSPLIT *f, char *buf);
extern void path_setcwd(const char *cwd);
extern void path_print(PATHSPLIT *f);
extern void donepath(void);

