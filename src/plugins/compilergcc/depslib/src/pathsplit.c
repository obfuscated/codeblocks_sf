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
#include "jam.h"
#include "pathsys.h"
#include "pathsplit.h"
#include "newstr.h"
#include <stdio.h>

static const char *_dot = ".";
static const char *_dotdot = "..";
#ifdef DEPSLIB_UNIX
static const char *_home = "~";
#endif
static const char *_cwd = 0;
static PATHSPLIT _fcwd;

void path_setcwd(const char *cwd)
{
	if (cwd)
	{
		_cwd = newstr(cwd);
		path_split(_cwd, &_fcwd);
	}
	else /* TODO: get cwd */
		cwd = 0;
}

void path_print(PATHSPLIT *f)
{
	int i;

	for (i = 0; i < f->count; i++)
		printf("'%.*s'[%d] ", f->part[i].len, f->part[i].ptr, f->part[i].len);
	printf("\n");
}

char *path_tostring(PATHSPLIT *f, char *buf)
{
	char *p = buf;
	int i;

	for (i = 0; i < f->count; i++)
	{
		memcpy(p, f->part[i].ptr, f->part[i].len);
		p += f->part[i].len;
		if (i + 1 < f->count)
		{
			*p++ = SEP1;
		}
	}
	*p = '\0';
#ifdef DOWNSHIFT_PATHS
	p = buf;
	do *p = tolower(*p);
	while (*p++);
#endif
	return buf;
}

void path_split(const char *path, PATHSPLIT *f)
{
	const char *p = path;
#ifdef DEPSLIB_WINDOWS
	/* support UNC filename in windows: ignore leading slashes */
	while ((*p == '/') || (*p == '\\'))
        ++p;
#endif
	PATHPART *part;

	f->count = 1;
	f->part[0].ptr = path;

	while (*p)
	{
		if ((*p == SEP1) || (*p == SEP2))
		{
			part = &f->part[f->count - 1];

			f->part[f->count].ptr = p + 1;
			part->len = p - part->ptr;

			if ((part->len == 1) && (part->ptr[0] == '.'))
				part->ptr = _dot;
			if ((part->len == 2) && (part->ptr[0] == '.') &&
				(part->ptr[1] == '.'))
				part->ptr = _dotdot;
#ifdef DEPSLIB_UNIX
			if ((part->len == 1) && (part->ptr[0] == '~'))
				part->ptr = _home;
#endif
			++f->count;
		}
		++p;
	}

	f->part[f->count - 1].len = p - f->part[f->count - 1].ptr;
}

/*
Windows:
 C:/foo/bar is absolute
 C:/./foo/bar is absolute
 C:/../foo/bar is absolute (but invalid)
 ./foo/bar is relative
 ../foo/bar is relative
 foo/bar is relative
Unix:
 /foo/bar is absolute
 /./foo/bar is absolute
 /../foo/bar is absolute (but invalid)
 ~/./foo/bar is absolute
 ~/../foo/bar is absolute
 ~/foo/bar is absolute
 ./foo/bar is relative
 ../foo/bar is relative
 foo/bar is relative
 */
int is_relative(PATHSPLIT *f)
{
#ifdef DEPSLIB_WINDOWS
	/* Volume C: D: etc is present, or is a UNC filename */
	if (((f->part[0].len == 2) && (f->part[0].ptr[1] == ':')) ||
		((f->part[0].len > 2) && (f->part[0].ptr[0] == '\\' && f->part[0].ptr[1] == '\\')))
	{
		return 0;
	}
#endif
#ifdef DEPSLIB_UNIX
	/* / or ~ is present */
	if ((f->part[0].len == 0) || (f->part[0].ptr == _home))
	{
		return 0;
	}
#endif
	return 1;
}

int path_normalize(PATHSPLIT *f, PATHSPLIT *cwd)
{
	PATHSPLIT f2;
	int i;

	if (is_relative(f) && (cwd || _cwd))
		f2 = cwd ? *cwd : _fcwd;
	else
		f2.count = 0;

	for (i = 0; i < f->count; i++)
	{
		PATHPART *part = &f->part[i];

		if (part->ptr == _dot)
			continue;

		if (part->ptr == _dotdot)
		{
			if (f2.count == 0)
				return 1;
			--f2.count;
			continue;
		}

		f2.part[f2.count].ptr = part->ptr;
		f2.part[f2.count].len = part->len;
		++f2.count;
	}

	*f = f2;
	return 0;
}

void donepath(void)
{
	_cwd = 0; /* freed in donestr() */
}
