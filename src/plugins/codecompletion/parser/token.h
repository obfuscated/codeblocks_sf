/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <wx/arrstr.h>
#include <wx/dynarray.h> // WX_DEFINE_ARRAY
#include <wx/string.h>

#include <deque>
#include <set>
#include <vector>

#include "searchtree.h"

class wxEvtHandler;

extern bool           g_EnableDebugTrace;
extern const wxString g_DebugTraceFile;

class Token;
class TokensTree;

enum FileParsingStatus
{
    fpsNotParsed = 0,
    fpsAssigned,
    fpsBeingParsed,
    fpsDone
};

WX_DEFINE_ARRAY(Token*, TokensArray);

typedef std::vector<Token*>                                      TokenList;
typedef std::deque<int>                                          TokenIdxList;
typedef std::set< int, std::less<int> >                          TokenIdxSet;
typedef SearchTree<TokenIdxSet>                                  TokenSearchTree;
typedef BasicSearchTree                                          TokenFilenamesMap;
typedef std::map< size_t, TokenIdxSet,       std::less<size_t> > TokenFilesMap;
typedef std::map< size_t, FileParsingStatus, std::less<size_t> > TokenFilesStatus;
typedef std::set< size_t, std::less<size_t> >                    TokenFilesSet;

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
friend class TokensTree;
public:
    Token(const wxString& name, unsigned int file, unsigned int line, size_t ticket);
    ~Token();

    bool AddChild(int childIdx);
    bool DeleteAllChildren();
    bool HasChildren() { return !m_Children.empty(); }
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
    bool MatchesFiles(const TokenFilesSet& files);

    TokensTree* GetTree() { return m_TokensTree; }
    bool IsValidAncestor(const wxString& ancestor);

    wxString                     m_Type;       // this is the return value (if any): e.g. const wxString&
    wxString                     m_ActualType; // this is what the parser believes is the actual return value: e.g. wxString
    wxString                     m_Name;
    wxString                     m_Args;
    wxString                     m_BaseArgs;
    wxString                     m_AncestorsString; // all ancestors comma-separated list
    wxString                     m_TemplateArgument;
    unsigned int                 m_FileIdx;
    unsigned int                 m_Line;
    unsigned int                 m_ImplFileIdx;
    unsigned int                 m_ImplLine;      // where the token was met
    unsigned int                 m_ImplLineStart; // if token is impl, opening brace line
    unsigned int                 m_ImplLineEnd;   // if token is impl, closing brace line
    TokenScope                   m_Scope;
    TokenKind                    m_TokenKind;
    bool                         m_IsOperator;
    bool                         m_IsLocal;       // found in a local file?
    bool                         m_IsTemp;        // local variable
    bool                         m_IsConst;       // the member method is const (yes/no)

    int                          m_Index;         // current index in the tree
    int                          m_ParentIndex;
    TokenIdxSet                  m_Children;
    TokenIdxSet                  m_Ancestors;
    TokenIdxSet                  m_DirectAncestors;
    TokenIdxSet                  m_Descendants;

    wxArrayString                m_Aliases; // used for namespace aliases

    wxArrayString                m_TemplateType;
    std::map<wxString, wxString> m_TemplateMap;

    wxString                     m_TemplateAlias; // alias for templates, e.g. template T1 T2;
    void*                        m_UserData; // custom user-data (the classbrowser expects it to be a pointer to a cbProject)

protected:
    TokensTree*                  m_TokensTree;
    size_t                       m_Ticket;
};

#endif // TOKEN_H
