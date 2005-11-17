//
// scWxString
//
// This class is used to pass strings between the application and the script engine.
// It is basically a container for the normal wxString, with the addition of a
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

#ifndef SC_WXSTRING_H
#define SC_WXSTRING_H

#include <wx/string.h>

class asIScriptEngine;

class scWxString
{
public:
	scWxString();
	scWxString(const scWxString &other);
	scWxString(const wxChar *s);
	scWxString(const wxString &s);

	void AddRef();
	void Release();

	scWxString &operator=(const scWxString &other);
	scWxString &operator+=(const scWxString &other);

	wxString buffer;

protected:
	~scWxString();
	int refCount;
};

// Call this function to register all the necessary
// functions for the scripts to use this type
void Register_wxString(asIScriptEngine *engine);

#endif // SC_WXSTRING_H
