/*
	see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
void *sq_vm_malloc(SQUnsignedInteger size){	return malloc(size); }

// C::B patch: Make the compiler happy by commenting unused variables
void *sq_vm_realloc(void *p, SQUnsignedInteger /*oldsize*/, SQUnsignedInteger size){ return realloc(p, size); }

// C::B patch: Make the compiler happy by commenting unused variables
void sq_vm_free(void *p, SQUnsignedInteger /*size*/){	free(p); }
