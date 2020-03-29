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

struct _alloc
{
	int size; /* size of one  element */
	int nel; /* number of elements in data */
	int free; /* number of free elements in data */
	ALLOC *tail; /* valid for head of chain only */
	ALLOC *next;
	char *data;
};

ALLOC *alloc_init(int size, int nel)
{
	ALLOC *a = malloc(sizeof(ALLOC));
	a->size = size;
	a->nel = a->free = nel;
	a->tail = 0;
	a->next = 0;
	a->data = malloc(size * nel);
	return a;
}

char *alloc_enter(ALLOC *chain)
{
	ALLOC *a = chain->tail ? chain->tail : chain;
	char *entry;

	if (!a->free)
	{
		a = alloc_init(chain->size, chain->nel);
		if (!chain->tail) chain->tail = chain;
		chain->tail->next = a;
		chain->tail = a;
	}

	entry = a->data + (a->nel - a->free) * a->size;
	--a->free;
	return entry;
}


ALLOC *alloc2_init(int size)
{
	ALLOC *a = malloc(sizeof(ALLOC));
	a->size = size;
	a->nel = 0;
	a->free = size;
	a->tail = 0;
	a->next = 0;
	a->data = malloc(size);
	return a;
}

char *alloc2_enter(ALLOC *chain, int size)
{
	ALLOC *a = chain->tail ? chain->tail : chain;
	char *entry;

	if (a->free < size)
	{
		a = alloc2_init(chain->size);
		if (!chain->tail) chain->tail = chain;
		chain->tail->next = a;
		chain->tail = a;
	}

	entry = a->data + a->size - a->free;
	a->free -= size;
	return entry;
}

void alloc_free(ALLOC *chain)
{
	while (chain)
	{
		ALLOC *next = chain->next;
		free(chain->data);
		free(chain);
		chain = next;
	}
}
