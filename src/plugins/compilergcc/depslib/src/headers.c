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
#include "alloc.h"
#include "hash.h"
#include "lists.h"
#include "newstr.h"
#include "regexp.h"
#include "headers.h"
#ifdef USE_CACHE
#include "cache.h"
#endif

#include "depslib.h" /* for struct depsStats */
extern struct depsStats g_stats;

struct hash *headerhash = 0;
static regexp *hdrre = 0;

LIST *headers1(const char *file)
{
	FILE *f;
	regexp *re;
	LIST *result = 0;
	char buf[1024];

	if (!(f = fopen(file, "r")))
		return result;

	if( DEBUG_HEADER )
	    printf("header scan %s\n", file);

	if (!hdrre)
		hdrre = my_regcomp("^[ 	]*#[ 	]*include[ 	]*([<\"])([^\">]*)([\">]).*$");
	re = hdrre;

	while (fgets(buf, sizeof(buf), f))
	{
		if (my_regexec(re, buf) && re->startp[3])
		{
			char buf2[MAXSYM];

			//FIXME: don't add duplicate headers
			int l = re->endp[3] - re->startp[1];
			memcpy(buf2, re->startp[1], l);
			buf2[l] = '\0';
			result = list_new(result, buf2, 0);

			if (DEBUG_HEADER)
				printf("header found: %s\n", buf2);
		}
	}

	fclose(f);

	g_stats.scanned++;

	return result;
}

static ALLOC *hdralloc = 0;

HEADERS *headerentry(HEADERS *chain, HEADER *header)
{
	HEADERS *c;

#if 1
	if (!hdralloc)
		hdralloc = alloc_init(sizeof(HEADERS), 64);
	c = (HEADERS *) alloc_enter(hdralloc);
#else
	c = (HEADERS *)malloc(sizeof(HEADERS));
#endif
	c->header = header;

	if (!chain) chain = c;
	else chain->tail->next = c;
	chain->tail = c;
	c->next = 0;

	return chain;
}

HEADER *headers(const char *t, time_t time)
{
	HEADER hdr, *h = &hdr;
	LIST *l;

	if (!headerhash)
		headerhash = hashinit(sizeof(HEADER), "headers");

	h->key = t;
	h->includes = 0;
	h->time = time;
	h->headers = 0;
	h->newest = 0;
	if (!hashenter(headerhash, (HASHDATA **)&h))
		return h;

	h->key = newstr(t);
#ifdef USE_CACHE
	if (!cache_check(t, time, &h->includes))
	{
		h->includes = headers1(t);
		cache_enter(t, time, h->includes);
	}
#else
	h->includes = headers1(t);
#endif

	l = h->includes;
	while (l)
	{
		const char *t2 = search(t, l->string, &time);
		if (time)
			h->headers = headerentry(h->headers, headers(t2, time));
		l = list_next(l);
	}

	return h;
}

void headernewest(HEADER *h)
{
	HEADERS *hs = h->headers;

	if (h->newest)
		return;
	h->newest = h;
	while (hs)
	{
		headernewest(hs->header);
		if (hs->header->newest->time > h->newest->time)
			h->newest = hs->header->newest;
		hs = hs->next;
	}
}

void donehdrs(void)
{
	my_redone(hdrre);
	hdrre = 0;
	hashdone(headerhash);
	headerhash = 0;
	alloc_free(hdralloc);
	hdralloc = 0;
}
