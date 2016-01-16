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
#include <stdio.h>
#include "jam.h"
#include "hash.h"
#include "lists.h"
#include "newstr.h"
#include "cache.h"

#include "depslib.h" /* for struct depsStats */
extern struct depsStats g_stats;

typedef struct _hdr HDR;

struct _hdr {
	const char *file;
	time_t time;
	LIST *includes;
	HDR *next;
	HDR *tail;
};

static struct hash *hdrhash = 0;
static HDR *hdrlist = 0;

static const char *magic = "# depslib dependency file v%d.%d";
#define DEPS_MAJOR 1
#define DEPS_MINOR 0

static HDR *hdr_append (HDR *l, HDR *nl)
{
	if (!nl)
	{
	}
	else if (!l)
	{
		l = nl;
	}
	else
	{
		l->tail->next = nl;
		l->tail = nl->tail;
	}

	return l;
}

static HDR *hdr_enter(const char *file)
{
	HDR hdr, *h = &hdr;

	if (!hdrhash)
		hdrhash = hashinit(sizeof (HDR), "headers");

	h->file = file;
	h->time = 0;
	h->includes = 0;
	h->next = 0;
	h->tail = 0;

	if (hashenter(hdrhash, (HASHDATA **)&h))
	{
		h->file = newstr(file);	/* never freed */
		h->tail = h;
		hdrlist = hdr_append(hdrlist, h);
	}

	return h;
}

#define CACHE_MISSING 0 /* no such file */
#define CACHE_OK 1 /* file is a depslib cache file */
#define CACHE_BAD 2 /* file is not a depslib cache file */

int check_cache_file(const char *path, int *vmajor, int *vminor)
{
	FILE *f;
	char buf[1024];
	int ret = CACHE_BAD;

	if (!(f = fopen(path, "r")))
		return CACHE_MISSING;

	if (!fgets(buf, sizeof(buf), f))
		goto done;
	if (sscanf(buf, magic, vmajor, vminor) != 2)
		goto done;
	ret = CACHE_OK;

done:
	fclose(f);
	return ret;
}

void cache_read(const char *path)
{
	FILE *f;
	char buf[1024];
	int vmajor, vminor;
	HDR *h = 0;
	int n;
	time_t timeval;

	if (check_cache_file(path, &vmajor, &vminor) != CACHE_OK)
		return;

	if ((vmajor != DEPS_MAJOR) || (vminor != DEPS_MINOR))
		return;

	if (!(f = fopen(path, "r")))
		return;

	/* Skip magic */
	fgets(buf, sizeof(buf), f);

	while (fgets(buf, sizeof (buf), f))
	{
		buf[strlen(buf) - 1] = '\0'; /* zap newline */

		if (!buf[0])
			continue;

		if (buf[0] == '\t')
		{
			h->includes = list_new(h->includes, buf + 1, 0);
			continue;
		}

		/* C::B patch: Compatibility with 64 bit compiler / OS */
		#if defined(_WIN64)
		sscanf(buf, "%I64d %n", &timeval, &n);
		#else
		sscanf(buf, "%ld %n", &timeval, &n);
		#endif
		h = hdr_enter (buf + n);
		h->time = timeval;
	}

	fclose(f);
}

void cache_write(const char *path)
{
	int vmajor, vminor;
	FILE *f;
	HDR *h;

	if (check_cache_file(path, &vmajor, &vminor) == CACHE_BAD)
		return;

	if (!(f = fopen(path, "w")))
		return;

	fprintf(f, magic, DEPS_MAJOR, DEPS_MINOR);
	fputc('\n', f);

	for (h = hdrlist; h; h = h->next)
	{
		LIST *l;
		/* C::B patch: Compatibility with 64 bit compiler / OS */
		#if defined(_WIN64)
		fprintf(f, "%I64d %s\n", h->time, h->file);
		#else
		fprintf(f, "%ld %s\n", h->time, h->file);
		#endif
		for (l = h->includes; l; l = list_next (l))
		{
			fprintf(f, "\t%s\n", l->string);
		}
		fprintf(f, "\n");
	}

	fclose(f);
}

int cache_check(const char *path, time_t time, LIST **includes)
{
	HDR *h;

	h = hdr_enter(path);
	if (h->time && (h->time == time))
	{
g_stats.cache_used++;
		*includes = h->includes;
		return 1;
	}
	return 0;
}

void cache_enter(const char *path, time_t time, LIST *includes)
{
	HDR *h;

	h = hdr_enter(path);
	h->time = time;
	h->includes = includes;
g_stats.cache_updated++;
}

void donecache(void)
{
	hashdone(hdrhash);
	hdrhash = 0;
	hdrlist = 0;
}

