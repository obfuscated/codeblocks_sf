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
 * 
 * Modifications:
 * - D support (search for "D support")
 */
#include "jam.h"
#include "hash.h"
#include "lists.h"
#include "newstr.h"
#include "pathsys.h"
#include "pathsplit.h"
#include "timestamp.h"
#include "search.h"

static LIST *searchdirs = 0;

#ifdef SEARCH_OPTIM
typedef struct _search SEARCH;
struct _search
{
	const char *key;
	const char *path;
	time_t time;
};
struct hash *searchhash = 0;
#endif

const char *search(const char *source, const char *_header, time_t *time)
{
	PATHNAME f[1];
	char buf[MAXJPATH];
	char buf2[MAXSYM], *header = buf2;
	char buf3[MAXJPATH];
	int system = (_header[0] == '<');
	LIST *list = searchdirs->next;
	
	//D support
	int dMode=0;
	int fnlen=strlen(source);
	if(source[fnlen-2]=='.' && source[fnlen-1]=='d')
		dMode=1;

	/* <foo.h> --> foo.h */
	strcpy(header, _header + 1);
	header[strlen(header) - 1] = '\0';

	/* src/foo.c --> src */
	path_parse(source, f);
	path_parent(f);
	path_build(f, buf3, 1);

	if (DEBUG_SEARCH)
		printf( "search %s\n  included by %s\n", _header, source);

#ifdef SEARCH_OPTIM
	{
		char key[MAXJPATH] = "";
		SEARCH search, *s = &search;
		if (!system)
		{
			strcpy(key, buf3);
			strcat(key, ",");
		}
		strcat(key, _header);
		s->key = key;
		if (!searchhash)
			searchhash = hashinit(sizeof(SEARCH), "search");
		if (hashcheck(searchhash, (HASHDATA **)&s))
		{
			if (DEBUG_SEARCH)
				printf("  %s: %s [CACHED]\n", _header, s->time ? s->path : "*missing*" );
			*time = s->time;
			return s->path;
		}
	}
#endif

	/* If this is "foo.h" not <foo.h> then set the first search directory
	 * to the including file's directory */
	if (!system)
	{
		searchdirs->string = buf3;
		list = searchdirs;
	}

	path_parse(header, f);

	f->f_grist.ptr = 0;
	f->f_grist.len = 0;
	
	for (; list; list = list->next)
	{
		f->f_root.ptr = list->string;
		f->f_root.len = strlen(list->string);

		path_build(f, buf, 1);

{
PATHSPLIT f;
char buf2[MAXJPATH];
path_split(buf, &f);
path_normalize(&f, NULL);
path_tostring(&f, buf2);
strcpy(buf, buf2);
}

		if (DEBUG_SEARCH)
			printf("  %s: %s [TRY]\n", _header, buf);

		timestamp(buf, time);

#ifdef SEARCH_OPTIM
		if (*time)
		{
			char key[MAXJPATH] = "";
			SEARCH search, *s = &search;
			if (!system)
			{
				strcpy(key, buf3);
				strcat(key, ",");
			}
			strcat(key, _header);
			s->key = newstr(key);
			s->time = *time;
			s->path = newstr(buf);
			(void) hashenter(searchhash, (HASHDATA **)&s);
		}
#endif

		if (*time)
			return newstr(buf);
	}

	if(!dMode)
	{
#ifdef SEARCH_OPTIM
		// remember that this file could not be found
		{
			char key[MAXJPATH] = "";
			SEARCH search, *s = &search;
			if (!system)
			{
				strcpy(key, buf3);
				strcat(key, ",");
			}
			strcat(key, _header);
			s->key = newstr(key);
			s->time = 0;
			s->path = NULL;
			(void) hashenter(searchhash, (HASHDATA **)&s);
		}
#endif

		/* C compilers do *not* look in the current directory for #include files */
		*time = 0;
		return NULL;
	}
	//D support (look in current directory)
	else
	{
		f->f_root.ptr = 0;
		f->f_root.len = 0;

		path_build( f, buf, 1 );

{
PATHSPLIT f;
char buf2[MAXJPATH];
path_split(buf, &f);
path_normalize(&f, NULL);
path_tostring(&f, buf2);
strcpy(buf, buf2);
}

		if( DEBUG_SEARCH )
			printf( "search %s: %s\n", _header, buf );

		timestamp( buf, time );


#ifdef SEARCH_OPTIM
		if (*time)
		{
			char key[MAXJPATH] = "";
			SEARCH search, *s = &search;
			if (!system)
			{
				strcpy(key, buf3);
				strcat(key, ",");
			}
			strcat(key, _header);
			s->key = newstr(key);
			s->time = *time;
			s->path = newstr(buf);
			(void) hashenter(searchhash, (HASHDATA **)&s);
		}
#endif

		if (*time)
			return newstr(buf);
		
#ifdef SEARCH_OPTIM
		// remember that this file could not be found
		{
			char key[MAXJPATH] = "";
			SEARCH search, *s = &search;
			if (!system)
			{
				strcpy(key, buf3);
				strcat(key, ",");
			}
			strcat(key, _header);
			s->key = newstr(key);
			s->time = 0;
			s->path = NULL;
			(void) hashenter(searchhash, (HASHDATA **)&s);
		}
#endif

		*time = 0;
		return NULL;
	}
}

void search_adddir(const char *path)
{
	PATHSPLIT f;
	char buf[MAXJPATH];

	path_split(path, &f);
	path_normalize(&f, NULL); // path_setcwd() should have been called
	path_tostring(&f, buf);
	searchdirs = list_new(searchdirs, buf, 0);

#ifdef SEARCH_OPTIM
	/* searchhash is only valid for a fixed set of search directories */
	hashdone(searchhash);
	searchhash = 0;
#endif
}

void search_init(void)
{
	donesearch();
	searchdirs = list_new(searchdirs, "SEARCH[0]", 0); /* required placeholder */
}

void donesearch(void)
{
	searchdirs = 0; /* freed by listdone() */
#ifdef SEARCH_OPTIM
	hashdone(searchhash);
	searchhash = 0;
#endif
}
