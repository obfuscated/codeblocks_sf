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
#include "depslib.h"

#include "jam.h"
#include "hash.h"
#include "lists.h"
#include "headers.h"
#include "newstr.h"
#include "pathsys.h"
#include "pathsplit.h"
#include "search.h"
#include "timestamp.h"
#ifdef USE_CACHE
#include "cache.h"
#endif

struct depsStats g_stats;

#define DEPS_STATE_INIT 0x0001
#define DEPS_STATE_DONE 0x0002
#define DEPS_STATE_SEARCH 0x0004

static int g_state = 0;

static int inState(int state)
{
	return (g_state & state) == state;
}

static int s_error = 0;

static void setErr(int error)
{
	s_error = error;
}

void depsStart(void)
{
	if (inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_ALREADY_INIT);
		return;
	}
	setErr(0);
	memset((char *) &g_stats, 0, sizeof(g_stats));
	g_state |= DEPS_STATE_INIT;
}

void depsSetCWD(const char *cwd)
{
	if (!inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	setErr(0);
	path_setcwd(cwd);
}

void depsSearchStart(void)
{
	if (!inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	search_init();
	g_state |= DEPS_STATE_SEARCH;
	setErr(0);
}

void depsAddSearchDir(const char *path)
{
	if (!inState(DEPS_STATE_INIT | DEPS_STATE_SEARCH))
	{
		if (!inState(DEPS_STATE_SEARCH)) setErr(DEPS_ERROR_NOT_SEARCH);
		if (!inState(DEPS_STATE_INIT)) setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	setErr(0);
	search_adddir(path);
}

depsRef depsScanForHeaders(const char *path)
{
	PATHSPLIT f;
	char buf[MAXJPATH];
	time_t time;
	HEADER *h;

	if (!inState(DEPS_STATE_INIT | DEPS_STATE_SEARCH))
	{
		if (!inState(DEPS_STATE_SEARCH)) setErr(DEPS_ERROR_NOT_SEARCH);
		if (!inState(DEPS_STATE_INIT)) setErr(DEPS_ERROR_NOT_INIT);
		return NULL;
	}
	setErr(0);

	path_split(path, &f);
	path_normalize(&f, NULL);
	path_tostring(&f, buf);

	timestamp(buf, &time);
	if (!time)
		return NULL;

	h = headers(buf, time);
	return (depsRef) h;
}

const char *depsGetNewest(depsRef ref, time_t *time)
{
	HEADER *h = (HEADER * )ref;

	headernewest(h);
	*time = h->newest->time;
	return h->newest->key;
}

void depsTimeStamp(const char *path, time_t *time)
{
	PATHSPLIT f;
	char buf[MAXJPATH];

	if (!inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	setErr(0);

	path_split(path, &f);
	path_normalize(&f, NULL);
	path_tostring(&f, buf);

	timestamp(buf, time);
}

void depsGetStats(struct depsStats *stats)
{
	if (!inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	setErr(0);

	*stats = g_stats;
}

void depsCacheRead(const char *path)
{
#ifdef USE_CACHE
	cache_read(path);
#endif
}

void depsCacheWrite(const char *path)
{
#ifdef USE_CACHE
	cache_write(path);
#endif
}

void depsDone(void)
{
	if (!inState(DEPS_STATE_INIT))
	{
		setErr(DEPS_ERROR_NOT_INIT);
		return;
	}
	setErr(0);

	donestamps();
	donestr();
	donelist();
	donehdrs();
	donesearch();
	donepath();
#ifdef USE_CACHE
	donecache();
#endif

	g_state = DEPS_STATE_DONE;
}

int depsError(void)
{
	return s_error;
}
