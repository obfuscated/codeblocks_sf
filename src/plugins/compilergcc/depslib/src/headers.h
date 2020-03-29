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
typedef struct _headers HEADERS;
typedef struct _header HEADER;

struct _headers
{
	HEADERS *next;
	HEADERS *tail;
	HEADER *header;
};

struct _header
{
	const char *key;
	LIST *includes;
	time_t time;
	HEADERS *headers;
	HEADER *newest;
};

extern HEADER *headers(const char *t, time_t time);
extern void headernewest(HEADER *h);
extern void donehdrs(void);
