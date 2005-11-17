//
// scWxArrayString
//
// This class is used to pass arraystrings between the application and the script engine.
// It is basically a container for the normal wxArrayString, with the addition of a
// reference counter so that the script can use object handles to hold the type.
//
// Because the class is reference counted it cannot be stored locally in the
// application functions, nor be received or returned by value. Instead it should
// be manipulated through pointers or references.
//
// Note, because the internal buffer is placed at the beginning of the class
// structure it is infact possible to receive this type as a reference or pointer
// to a normal wxString where the reference counter doesn't have to be manipulated.
//

#ifndef SC_WXARRAYSTRING_H
#define SC_WXARRAYSTRING_H

#include <wx/arrstr.h>

class asIScriptEngine;

class scWxArrayString
{
public:
	scWxArrayString();
	scWxArrayString(const scWxArrayString &other);

	void AddRef();
	void Release();

	wxArrayString buffer;

protected:
	~scWxArrayString();
	int refCount;
};

// Call this function to register all the necessary
// functions for the scripts to use this type
void Register_wxArrayString(asIScriptEngine *engine);

#endif // SC_WXARRAYSTRING_H
