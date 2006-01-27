#include <sdk_precomp.h>
#include <new>
#include <angelscript.h>
#include "sc_wxstring.h"
#include <globals.h>

//--------------
// constructors
//--------------

scWxString::scWxString()
{
	// Count the first reference
	refCount = 1;
}

scWxString::scWxString(const wxChar *s)
{
	refCount = 1;
	buffer = s;
}

scWxString::scWxString(const wxString &s)
{
	refCount = 1;
	buffer = s;
}

scWxString::scWxString(const scWxString &s)
{
	refCount = 1;
	buffer = s.buffer;
}

scWxString::~scWxString()
{
	assert( refCount == 0 );
}

//--------------------
// reference counting
//--------------------

void scWxString::AddRef()
{
	refCount++;
}

void scWxString::Release()
{
	if( --refCount == 0 )
		delete this;
}

//-----------------
// wxString = wxString
//-----------------

scWxString &scWxString::operator=(const scWxString &other)
{
	// Copy only the buffer, not the reference counter
	buffer = other.buffer;

	// Return a reference to this object
	return *this;
}

//------------------
// wxString += wxString
//------------------

scWxString &scWxString::operator+=(const scWxString &other)
{
	buffer += other.buffer;
	return *this;
}

//-----------------
// wxString + wxString
//-----------------

scWxString *operator+(const scWxString &a, const scWxString &b)
{
	// Return a new object as a script handle
	return new scWxString(a.buffer + b.buffer);
}

//----------------
// wxString = value
//----------------

static scWxString &AssignBitsToString(asDWORD i, scWxString &dest)
{
	dest.buffer.Printf(_T("%ld"), i);

	// Return a reference to the object
	return dest;
}

static scWxString &AssignCharToString(char i, scWxString &dest)
{
	dest.buffer.Printf(_T("%c"), i);
	return dest;
}

static scWxString &AssignUIntToString(unsigned int i, scWxString &dest)
{
	dest.buffer.Printf(_T("%ud"), i);
	return dest;
}

static scWxString &AssignIntToString(int i, scWxString &dest)
{
	dest.buffer.Printf(_T("%d"), i);
	return dest;
}

static scWxString &AssignFloatToString(float f, scWxString &dest)
{
	dest.buffer.Printf(_T("%f"), f);
	return dest;
}

static scWxString &AssignDoubleToString(double f, scWxString &dest)
{
	dest.buffer.Printf(_T("%f"), f); // TODO: 'double' specifier?
	return dest;
}

//-----------------
// wxString += value
//-----------------

static scWxString &AddAssignBitsToString(asDWORD i, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%ld"), i);
	return dest;
}

static scWxString &AddAssignCharToString(char i, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%c"), i);
	return dest;
}

static scWxString &AddAssignUIntToString(unsigned int i, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%ud"), i);
	return dest;
}

static scWxString &AddAssignIntToString(int i, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%d"), i);
	return dest;
}

static scWxString &AddAssignFloatToString(float f, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%f"), f);
	return dest;
}

static scWxString &AddAssignDoubleToString(double f, scWxString &dest)
{
	dest.buffer << wxString::Format(_T("%f"), f);
	return dest;
}

//----------------
// wxString + value
//----------------

static scWxString *AddStringBits(const scWxString &str, asDWORD i)
{
	return new scWxString(str.buffer + wxString::Format(_T("%ld"), i));
}

static scWxString *AddStringChar(const scWxString &str, char i)
{
	return new scWxString(str.buffer + wxString::Format(_T("%c"), i));
}

static scWxString *AddStringUInt(const scWxString &str, unsigned int i)
{
	return new scWxString(str.buffer + wxString::Format(_T("%ud"), i));
}

static scWxString *AddStringInt(const scWxString &str, int i)
{
	return new scWxString(str.buffer + wxString::Format(_T("%d"), i));
}

static scWxString *AddStringFloat(const scWxString &str, float f)
{
	return new scWxString(str.buffer + wxString::Format(_T("%f"), f));
}

static scWxString *AddStringDouble(const scWxString &str, double f)
{
	return new scWxString(str.buffer + wxString::Format(_T("%f"), f));
}

//----------------
// value + wxString
//----------------

static scWxString *AddBitsString(asDWORD i, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%ld"), i) + str.buffer);
}

static scWxString *AddIntString(int i, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%d"), i) + str.buffer);
}

static scWxString *AddCharString(char i, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%c"), i) + str.buffer);
}

static scWxString *AddUIntString(unsigned int i, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%ud"), i) + str.buffer);
}

static scWxString *AddFloatString(float f, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%f"), f) + str.buffer);
}

static scWxString *AddDoubleString(double f, const scWxString &str)
{
	return new scWxString(wxString::Format(_T("%f"), f) + str.buffer);
}

//----------
// wxString[]
//----------

static char *StringCharAt(unsigned int i, scWxString &str)
{
	if( i >= str.buffer.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		return 0;
	}

    const char* s = _C(str.buffer);
    return (char*)&s[i];
//    static char c = str.buffer.GetChar(i);
//	return &c;
}

//-----------------------
// AngelScript functions
//-----------------------

// This function allocates memory for the wxString object
static void *StringAlloc(int)
{
	return new char[sizeof(scWxString)];
}

// This function deallocates the memory for the wxString object
static void StringFree(void *p)
{
	assert( p );
	delete (char*)p;
}

// This is the wxString factory that creates new strings for the script
static scWxString *StringFactory(asUINT length, const char *s)
{
	// Return a script handle to a new wxString
	return new scWxString(_U(s));
}

// This is a wrapper for the default scWxString constructor, since
// it is not possible to take the address of the constructor directly
static void ConstructString(scWxString *thisPointer)
{
	// Construct the wxString in the memory received
	new(thisPointer) scWxString();
}

// This is where we register the wxString type
void Register_wxString(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("wxString", sizeof(scWxString), asOBJ_CLASS_CDA); assert( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADDREF,     "void f()",                    asMETHOD(scWxString,AddRef), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_RELEASE,    "void f()",                    asMETHOD(scWxString,Release), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(const wxString &in)", asMETHODPR(scWxString, operator =, (const scWxString&), scWxString&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(const wxString &in)", asMETHODPR(scWxString, operator+=, (const scWxString&), scWxString&), asCALL_THISCALL); assert( r >= 0 );

	// Register the memory allocator routines. This will make all memory allocations for the wxString
	// object be made in one place, which is important if for example the script library is used from a dll
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ALLOC, "wxString &f(uint)", asFUNCTION(StringAlloc), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_FREE, "void f(wxString &in)", asFUNCTION(StringFree), asCALL_CDECL); assert( r >= 0 );

	// Register the factory to return a handle to a new wxString
	// Note: We must register the wxString factory after the basic behaviours,
	// otherwise the library will not allow the use of object handles for this type
	r = engine->RegisterStringFactory("wxString@", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Register the global operator overloads
	// Note: We can use wxString's methods directly because the
	// internal wxString is placed at the beginning of the class
	r = engine->RegisterGlobalBehaviour(asBEHAVE_EQUAL,       "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator==, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_NOTEQUAL,    "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator!=, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LEQUAL,      "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator<=, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GEQUAL,      "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator>=, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LESSTHAN,    "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator <, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GREATERTHAN, "bool f(const wxString &in, const wxString &in)",    asFUNCTIONPR(operator >, (const wxString &, const wxString &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, const wxString &in)", asFUNCTIONPR(operator +, (const scWxString &, const scWxString &), scWxString*), asCALL_CDECL); assert( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_INDEX, "uint8 &f(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_INDEX, "const uint8 &f(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("wxString", "uint length() const", asMETHOD(wxString,size), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "uint Length() const", asMETHOD(wxString,size), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "uint Len() const", asMETHOD(wxString,size), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "uint size() const", asMETHOD(wxString,size), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "uint Replace(const wxString& in,const wxString& in,bool)", asMETHOD(wxString,Replace), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "wxString BeforeFirst(uint8) const", asMETHOD(wxString,BeforeFirst), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "wxString AfterFirst(uint8) const", asMETHOD(wxString,AfterFirst), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "wxString BeforeLast(uint8) const", asMETHOD(wxString,BeforeLast), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "wxString AfterLast(uint8) const", asMETHOD(wxString,AfterLast), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "int Find(uint8,bool) const", asMETHODPR(wxString,Find,(wxChar, bool) const, int), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("wxString", "int Find(const wxString& in) const", asMETHODPR(wxString,Find,(const wxChar*) const, int), asCALL_THISCALL); assert( r >= 0 );

	// Automatic conversion from values
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, double)", asFUNCTION(AddStringDouble), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(double, const wxString &in)", asFUNCTION(AddDoubleString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(float)", asFUNCTION(AssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(float)", asFUNCTION(AddAssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, float)", asFUNCTION(AddStringFloat), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(float, const wxString &in)", asFUNCTION(AddFloatString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, int)", asFUNCTION(AddStringInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(int, const wxString &in)", asFUNCTION(AddIntString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, uint)", asFUNCTION(AddStringUInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(uint, const wxString &in)", asFUNCTION(AddUIntString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(uint8)", asFUNCTION(AssignCharToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(uint8)", asFUNCTION(AddAssignCharToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, uint8)", asFUNCTION(AddStringChar), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(uint8, const wxString &in)", asFUNCTION(AddCharString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ASSIGNMENT, "wxString &f(bits)", asFUNCTION(AssignBitsToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("wxString", asBEHAVE_ADD_ASSIGN, "wxString &f(bits)", asFUNCTION(AddAssignBitsToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(const wxString &in, bits)", asFUNCTION(AddStringBits), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "wxString@ f(bits, const wxString &in)", asFUNCTION(AddBitsString), asCALL_CDECL); assert( r >= 0 );
}
