#include <sdk_precomp.h>
#include <new>
#include <angelscript.h>
#include "sc_wxarraystring.h"

#define AS_ASSERT wxASSERT

//--------------
// constructors
//--------------

scWxArrayString::scWxArrayString()
{
	// Count the first reference
	refCount = 1;
}

scWxArrayString::scWxArrayString(const scWxArrayString &s)
{
	refCount = 1;
	buffer = s.buffer;
}

scWxArrayString::~scWxArrayString()
{
	AS_ASSERT( refCount == 0 );
}

//--------------------
// reference counting
//--------------------

void scWxArrayString::AddRef()
{
	refCount++;
}

void scWxArrayString::Release()
{
	if( --refCount == 0 )
		delete this;
}

//----------
// wxArrayString[]
//----------

static wxString& StringAt(unsigned int i, scWxArrayString &str)
{
	if( i >= str.buffer.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		static wxString s;
		return s;
	}

	return str.buffer[i];
}

//-----------------------
// AngelScript functions
//-----------------------

// This function allocates memory for the wxArrayString object
static void *ArrayStringAlloc(int)
{
	return new char[sizeof(scWxArrayString)];
}

// This function deallocates the memory for the wxArrayString object
static void ArrayStringFree(void *p)
{
	AS_ASSERT( p );
	delete (char*)p;
}

// This is a wrapper for the default scWxArrayString constructor, since
// it is not possible to take the address of the constructor directly
static void ConstructArrayString(scWxArrayString *thisPointer)
{
	// Construct the wxArrayString in the memory received
	new(thisPointer) scWxArrayString();
}

// This is where we register the wxArrayString type
void Register_wxArrayString(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("wxArrayString", sizeof(scWxArrayString), asOBJ_CLASS_CDA); AS_ASSERT( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructArrayString), asCALL_CDECL_OBJLAST); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_ADDREF,     "void f()",                    asMETHOD(scWxArrayString,AddRef), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_RELEASE,    "void f()",                    asMETHOD(scWxArrayString,Release), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_ASSIGNMENT, "wxArrayString &f(const wxArrayString &in)", asMETHODPR(scWxArrayString, operator =, (const scWxArrayString&), scWxArrayString&), asCALL_THISCALL); AS_ASSERT( r >= 0 );

	// Register the memory allocator routines. This will make all memory allocations for the wxArrayString
	// object be made in one place, which is important if for example the script library is used from a dll
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_ALLOC, "wxArrayString &f(uint)", asFUNCTION(ArrayStringAlloc), asCALL_CDECL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_FREE, "void f(wxArrayString &in)", asFUNCTION(ArrayStringFree), asCALL_CDECL); AS_ASSERT( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_INDEX, "wxString &f(uint)", asFUNCTION(StringAt), asCALL_CDECL_OBJLAST); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxArrayString", asBEHAVE_INDEX, "const wxString &f(uint) const", asFUNCTION(StringAt), asCALL_CDECL_OBJLAST); AS_ASSERT( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("wxArrayString", "uint Add(const wxString& in)", asMETHOD(wxArrayString,Add), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Alloc(int)", asMETHOD(wxArrayString,Alloc), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Clear()", asMETHOD(wxArrayString,Clear), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "uint GetCount() const", asMETHOD(wxArrayString,Count), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "int Index(const wxString& in, bool, bool)", asMETHOD(wxArrayString,Index), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Insert(const wxString& in, uint)", asMETHOD(wxArrayString,Insert), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "bool IsEmpty()", asMETHOD(wxArrayString,IsEmpty), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "wxString& Item(uint) const", asMETHOD(wxArrayString,Item), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "wxString& Last()", asMETHOD(wxArrayString,Last), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Remove(const wxString& in)", asMETHODPR(wxArrayString,Remove,(const wxChar*),void), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void RemoveAt(uint,uint) const", asMETHOD(wxArrayString,RemoveAt), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Shrink()", asMETHOD(wxArrayString,Shrink), asCALL_THISCALL); AS_ASSERT( r >= 0 );
	r = engine->RegisterObjectMethod("wxArrayString", "void Sort(bool)", asMETHODPR(wxArrayString,Sort,(bool),void), asCALL_THISCALL); AS_ASSERT( r >= 0 );
}
