#ifndef TOKEN_H
#define TOKEN_H

#include <wx/string.h>
#include <wx/dynarray.h>

class Token;
WX_DEFINE_ARRAY(Token*, TokensArray);

enum TokenScope
{
	tsUndefined = 0,
	tsPrivate,
	tsProtected,
	tsPublic
};

enum TokenKind
{
	tkClass         = 0x0001,
	tkNamespace     = 0x0002,
	tkConstructor   = 0x0004,
	tkDestructor    = 0x0008,
	tkFunction      = 0x0010,
	tkVariable      = 0x0020,
	tkEnum          = 0x0040,
	tkEnumerator    = 0x0080,
	tkPreprocessor  = 0x0100,
	tkUndefined     = 0xFFFF,
};

class Token
{
	public:
		Token();
		Token(const wxString& name, const wxString& filename, unsigned int line);
		virtual ~Token();

		virtual void AddChild(Token* child);
		wxString GetNamespace();
		bool InheritsFrom(Token* token);
		wxString GetTokenKindString();
		wxString GetTokenScopeString();

		wxString m_Type; // this is the return value (if any): e.g. const wxString&
		wxString m_ActualType; // this is what the parser believes is the actual return value: e.g. wxString
		wxString m_Name;
		wxString m_DisplayName;
		wxString m_Args;
		wxString m_AncestorsString; // all ancestors comma-separated list
		wxString m_Filename;
		unsigned int m_Line;
		wxString m_ImplFilename;
		unsigned int m_ImplLine;
		TokenScope m_Scope;
		TokenKind m_TokenKind;
		bool m_IsOperator;
		bool m_IsLocal; // found in a local file?
		bool m_IsTemporary;
		TokensArray m_Ancestors; // after Parser::LinkInheritance() runs, contains all ancestor tokens (affected by m_AncestorsString)

		wxString m_String; // custom string value
		bool m_Bool; // custom bool value
		int m_Int; // custom int value
		void* m_Data; // custom pointer value

		Token* m_pParent;
		TokensArray m_Children;
	protected:
	private:
};

#endif // TOKEN_H

