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
typedef struct _alloc ALLOC;

extern ALLOC *alloc_init(int size, int nel);
extern char *alloc_enter(ALLOC *chain);
extern ALLOC *alloc2_init(int size);
extern char *alloc2_enter(ALLOC *chain, int size);
extern void alloc_free(ALLOC *chain);
