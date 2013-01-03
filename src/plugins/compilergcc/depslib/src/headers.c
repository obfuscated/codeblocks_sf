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
 * - Depth level counting (needed for D support)
 * - Simple optimization by avoiding regexec most of the time
 * - Special cache keys for source files (needed for D support)
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
//D support
static regexp *dimpre = 0;

LIST *headers1(const char *file, int depth)
{
	FILE *f;
	regexp *re;
	LIST *result = 0;
	char buf[1024];
	int fnlen=strlen(file);
	
	//D support
	int dMode=0;
	int dState=0;
	if(file[fnlen-2] == '.' && file[fnlen-1] == 'd')
	{
		dMode=1;
		if( DEBUG_HEADER )
			printf("D file detected\n");
	}

	// C::B patch: Debug usage of root folder
	if( DEBUG_HEADER )
	    printf("header open %s\n", file);

	if (!(f = fopen(file, "r")))
		return result;

	if( DEBUG_HEADER )
	    printf("header scan %s\n", file);

	if (!hdrre)
		hdrre = my_regcomp("^[ 	]*#[ 	]*include[ 	]*([<\"])([^\">]*)([\">]).*$");
	re = hdrre;
	
	//D support
	if(dMode)
	{
		if(!dimpre)
			dimpre = my_regcomp(
				"^.*import[ \t]*([[A-Za-z_ \t]+=[ \t]*)?([A-Za-z_\\.]+)(\\:.+)?;.*$");
		re = dimpre;
	}
	
	while (fgets(buf, sizeof(buf), f))
	{
		//D support
		if(dMode)
		{
			if(dState == 0)
			{
				if(strstr(buf, "public"))
					dState=1;
			}
			if(dState >= 1)
			{
				if(strchr(buf, '{'))
					++dState;
				if(strchr(buf, '}'))
					--dState;
			}
		}
		
		//Simple reduction of regex overhead
		if(strstr(buf, dMode ? "import" : "include"))
			if (my_regexec(re, buf))
			{
				char buf2[MAXSYM];

				//FIXME: don't add duplicate headers
				if(!dMode && re->startp[3])
				{
					int l = re->endp[3] - re->startp[1];
					memcpy(buf2, re->startp[1], l);
					buf2[l] = '\0';
				}
				//D support
				else if(re->startp[2])
				{
					if(depth > 0)
					{
						//private import?
						if(dState == 0)
							continue;
					}
					
					buf2[0] = '<';
					const char* p;
					int j = 0;
					for(p = re->startp[2];p < re->endp[2];++p)
					{
						char c=*p;
						++j;
						if(c == '.')
							buf2[j] = '/';
						else buf2[j] = c;
					}
					buf2[++j]='.';
					buf2[++j]='d';
					buf2[++j]='>';
					buf2[++j] = '\0';
				}
				result = list_new(result, buf2, 0);

				if (DEBUG_HEADER)
					printf("header found: %s\n", buf2);
			}
		
		//D support
		if(dMode)
		{
			if(dState == 1)
			{
				if(strchr(buf, ';'))
					dState=0;
				
				if(strchr(buf, '}'))
					dState=0;
			}
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

HEADER *headersDepth(const char *t, time_t time, int depth)
{
	HEADER hdr, *h = &hdr;
	LIST *l;
	const char* cachekey=t;

	//D support (doesn't affect C(++), because a source file is never included)
	if(depth == 0)
	{
		cachekey=malloc(strlen(t)+sizeof("source:"));
		strcpy((char*)cachekey,"source:");
		strcpy((char*)cachekey+7,t);
	}
	
	if (!headerhash)
		headerhash = hashinit(sizeof(HEADER), "headers");

	h->key = cachekey;
	h->includes = 0;
	h->time = time;
	h->headers = 0;
	h->newest = 0;
	if (!hashenter(headerhash, (HASHDATA **)&h))
		return h;

	h->key = newstr(t);
#ifdef USE_CACHE
	if (!cache_check(cachekey, time, &h->includes))
	{
		h->includes = headers1(t, depth);
		cache_enter(cachekey, time, h->includes);
	}
#else
	h->includes = headers1(t, depth);
#endif
	if(depth == 0)
		free((char*)cachekey);

	l = h->includes;
	while (l)
	{
		const char *t2 = search(t, l->string, &time);
		if (time)
			h->headers = headerentry(h->headers, headersDepth(t2, time, depth+1));
		l = list_next(l);
	}

	return h;
}

HEADER *headers(const char *t, time_t time)
{
	return headersDepth(t, time, 0);
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
	//D support
	my_redone(dimpre);
	dimpre = 0;
	hashdone(headerhash);
	headerhash = 0;
	alloc_free(hdralloc);
	hdralloc = 0;
}
