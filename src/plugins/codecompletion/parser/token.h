/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <wx/arrstr.h>
#include <wx/string.h>

#include <set>
#include <map>

class Token;
class TokenTree;

typedef std::set< int,    std::less<int>    > TokenIdxSet;
typedef std::set< size_t, std::less<size_t> > TokenFileSet;

enum TokenScope
{
    tsUndefined = 0,
    tsPrivate,
    tsProtected,
    tsPublic
};

enum TokenKind
{
    // changed in order to reflect the priority
    tkNamespace     = 0x0001,
    tkClass         = 0x0002,
    tkEnum          = 0x0004,
    tkTypedef       = 0x0008, // typedefs are stored as classes inheriting from the typedef'd type (taking advantage of existing inheritance code)
    tkConstructor   = 0x0010,
    tkDestructor    = 0x0020,
    tkFunction      = 0x0040,
    tkVariable      = 0x0080,
    tkEnumerator    = 0x0100,
    tkPreprocessor  = 0x0200,
    tkMacro         = 0x0400,

    // convenient masks
    tkAnyContainer  = tkClass    | tkNamespace   | tkTypedef,
    tkAnyFunction   = tkFunction | tkConstructor | tkDestructor,

    // undefined or just "all"
    tkUndefined     = 0xFFFF
};

class Token
{
friend class TokenTree;
public:
    Token(const wxString& name, unsigned int file, unsigned int line, size_t ticket);
    ~Token();

    bool AddChild(int childIdx);
    bool DeleteAllChildren();
    bool HasChildren() const { return !m_Children.empty(); }
    wxString GetNamespace() const;
    bool InheritsFrom(int idx) const;
    wxString DisplayName() const;
    wxString GetTokenKindString() const;
    wxString GetTokenScopeString() const;
    wxString GetFilename() const;
    wxString GetImplFilename() const;
    wxString GetFormattedArgs() const; // remove all '\n'
    wxString GetStrippedArgs() const; // remove all default value
    size_t GetTicket() const { return m_Ticket; }
    bool MatchesFiles(const TokenFileSet& files);

    TokenTree* GetTree() const { return m_TokenTree; }
    bool IsValidAncestor(const wxString& ancestor);

    wxString                     m_FullType; // this is the full return value (if any): e.g. const wxString&
    wxString                     m_BaseType; // this is what the parser believes is the actual return value: e.g. wxString
    wxString                     m_Name;     // Token's name, it can be searched in the TokenTree
    wxString                     m_Args;     // If it is a function Token, then this value is function arguments
                                             // e.g.   (int arg1 = 10, float arg2 = 9.0)
                                             // If it is an enumerator, then this is the assigned (inferred) value
    wxString                     m_BaseArgs; // stripped arguments e.g. (int arg1, float arg2)

    wxString                     m_AncestorsString; // all ancestors comma-separated list

    unsigned int                 m_FileIdx;       // File index in TokenTree
    unsigned int                 m_Line;          // Line index where the token was met, which is 0/1 based
    unsigned int                 m_ImplFileIdx;   // function implementation file index
    unsigned int                 m_ImplLine;      // function implementation line index
    unsigned int                 m_ImplLineStart; // if token is impl, opening brace line
    unsigned int                 m_ImplLineEnd;   // if token is impl, closing brace line

    TokenScope                   m_Scope;         // public? private? protected?
    TokenKind                    m_TokenKind;     // See TokenKind class
    bool                         m_IsOperator;    // is operator overload function?
    bool                         m_IsLocal;       // found in a local source file, otherwise in wxString
    bool                         m_IsTemp;        // local (automatic) variable
    bool                         m_IsConst;       // the member method is const (yes/no)
    bool                         m_IsAnonymous;   // Is anonymous token? (e.g. unnamed struct or union)

    int                          m_Index;         // current Token index in the tree
    int                          m_ParentIndex;   // Parent Token index
    TokenIdxSet                  m_Children;      // if it is a class kind token, then it contains all the member tokens
    TokenIdxSet                  m_Ancestors;     // all the ancestors in the inheritance hierarchy
    TokenIdxSet                  m_DirectAncestors; //the neatest ancestors
    TokenIdxSet                  m_Descendants;   // all the descendants in the inheritance hierarchy

    wxArrayString                m_Aliases;       // used for namespace aliases

    wxString                     m_TemplateArgument;
    wxArrayString                m_TemplateType;    //for a class template, this is the formal template argument list
                                                    //but for a variable Token, this is the actual template arguments.
    std::map<wxString, wxString> m_TemplateMap;

    wxString                     m_TemplateAlias;   // alias for templates, e.g. template T1 T2;
    void*                        m_UserData;        // custom user-data (the classbrowser expects it to be a pointer to a cbProject)

protected:
    TokenTree*                   m_TokenTree;       // a pointer to TokenTree
    size_t                       m_Ticket;          // This is used in classbrowser to avoid duplication
};

#endif // TOKEN_H
