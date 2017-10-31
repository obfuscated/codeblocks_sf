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
#define SEARCH_OPTIM

extern const char *search(const char *source, const char *_header, time_t *time);
extern void search_adddir(const char *path);
extern void search_init(void);
extern void donesearch(void);

