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

    /** namespace*/
    tkNamespace        = 0x0001,

    /** class or struct */
    tkClass            = 0x0002,

    /** enum */
    tkEnum             = 0x0004,

    /** typedef, note typedefs are stored as classes inheriting from the typedef'd type, this takes
     *  advantage of existing inheritance code
     */
    tkTypedef          = 0x0008,

    /** constructor class member function */
    tkConstructor      = 0x0010,

    /** destructor class member function */
    tkDestructor       = 0x0020,

    /** general function, not constructor nor destructor */
    tkFunction         = 0x0040,

    /** variable */
    tkVariable         = 0x0080,

    /** enumerator */
    tkEnumerator       = 0x0100,

    /** macro definition, such as: #define AAA(x,y) f(x,y), where AAA is a token of tkMacroDef */
    tkMacroDef         = 0x0200,

    /** the usage of the macro, for example: AAA(1,2) */
    tkMacroUse         = 0x0400,

    // convenient masks
    /** container like tokens, those tokens can have children tokens */
    tkAnyContainer     = tkClass    | tkNamespace   | tkTypedef,

    /** any kind of functions */
    tkAnyFunction      = tkFunction | tkConstructor | tkDestructor,

    /** undefined or just "all" */
    tkUndefined        = 0xFFFF
};
/** a symbol found in the parsed files, it can be many kinds, such as a variable, a class and so on.
 *  Once constructed, the Token should be always put in the TokenTree, and we can access the Token
 *  by TokenIndex.
 */
class Token
{
friend class TokenTree;
public:

    /** constructor
     *  @param name token's name, this can be a search key in the tokentree
     *  @param file the index of the source file where the token locates
     *  @param ticket an integer number, once a new Token is allocated, this value will increase by
     *  one, it is mainly used for duplicated Token checking when class browser tree get refreshed.
     */
    Token(const wxString& name, unsigned int file, unsigned int line, size_t ticket);

    /** destructor */
    ~Token();

    /** add a child token
     * @param childIdx int
     * @return return true if success, in the case the childIdx < 0, this function will return false
     */
    bool AddChild(int childIdx);

    /** delete all the child tokens of the current token, not only remove the relation, but also
     *  delete the Token instance.
     * @return false if the TokenTree is invalid
     */
    bool DeleteAllChildren();

    /** check if the token has any child tokens.
     * @return true if it does have some child.
     */
    bool HasChildren() const { return !m_Children.empty(); }

    /** @brief get a literal string presentation of the namespace.
     *
     * @return wxString
     * if the token has the parent token, like a member variable class AAA {int m_BBB;}, then
     * the string is "AAA::"
     */
    wxString GetNamespace() const;

    /** @brief check to see the current token is inherited from a specified token
     *
     * @param idx the specified token
     * @return true if success
     * loop the m_DirectAncestors to see it did contains the specified token, also we may check
     * the specified token is an ancestor of the current ancestor, so recursive call is involved
     */
    bool InheritsFrom(int idx) const;

    /** a short simple string to show the token information, this usually generate for show
     * the tip message when the user hover a mouse over the text in C::B editor.
     */
    wxString DisplayName() const;

    /** the token kind string, e.g. for a tkClass kind Token, we get a "class" string */
    wxString GetTokenKindString() const;

    /** the access kind string, e.g. for a tsPrivate type Token, we get a "private" string */
    wxString GetTokenScopeString() const;

    /** get a full path of the file which contains the current Token */
    wxString GetFilename() const;

    /** get a full path of the file which contains the function implementation. Note usually only
     *  function like token has this feature.
     */
    wxString GetImplFilename() const;

    /** remove all '\n' in the original function argument string */
    wxString GetFormattedArgs() const;

    /** remove all default value of the function argument string, e.g. if we have such argument
     *  "(int a = 10, float b = 3.14)"
     *  then we get "(int a, float b)"
     */
    wxString GetStrippedArgs() const;

    /** get the ticket value of the current token */
    size_t GetTicket() const { return m_Ticket; }

    /** see whether the current token belong to any files in the file set, both m_FileIdx and
     * m_ImplFileIdx is considered
     * @param files a file(index) set
     * @return true if success
     */
    bool MatchesFiles(const TokenFileSet& files);

    /** get the TokenTree associated with the current Token
     * @return TokenTree pointer
     */
    TokenTree* GetTree() const { return m_TokenTree; }

    /** build in types are not valid ancestors for a type define token
     * @param ancestor testing type string
     * @return false if the testing type string is a build in type
     */
    bool IsValidAncestor(const wxString& ancestor);

    /** this is the full return value (if any): e.g. const wxString& */
    wxString                     m_FullType;

    /** this is what the parser believes is the actual return value: e.g. wxString */
    wxString                     m_BaseType;

    /** Token's name, it can be searched in the TokenTree */
    wxString                     m_Name;

    /** If it is a function Token, then this value is function arguments,
     *  e.g.   (int arg1 = 10, float arg2 = 9.0)
     *  If it is an enumerator, then this is the assigned (inferred) value
     */
    wxString                     m_Args;

    /** stripped arguments e.g. (int arg1, float arg2) */
    wxString                     m_BaseArgs;

    /** all ancestors comma-separated list, e.g. if a class declaration like below
     *  class AAA :public BBB, public CCC
     *  then the m_AncestorsString is "BBB,CCC", note that once m_Ancestors is constructed, this
     *  variable will be cleared.
     */
    wxString                     m_AncestorsString;

    /** File index in TokenTree */
    unsigned int                 m_FileIdx;

    /** Line index where the token was met, which is 1 based */
    unsigned int                 m_Line;

    /** function implementation file index */
    unsigned int                 m_ImplFileIdx;

    /** function implementation line index */
    unsigned int                 m_ImplLine;

    /** if token is impl, opening brace line */
    unsigned int                 m_ImplLineStart;

    /** if token is impl, closing brace line */
    unsigned int                 m_ImplLineEnd;

    /** public? private? protected? */
    TokenScope                   m_Scope;

    /** See TokenKind class */
    TokenKind                    m_TokenKind;

    /** is operator overload function? */
    bool                         m_IsOperator;

    /** if true, means the token belong to a C::B project, it exists in the project's source/header
     *  files, not from the system's headers or other include header files
     */
    bool                         m_IsLocal;

    /** local (automatic) variable */
    bool                         m_IsTemp;

    /** the member method is const (yes/no) */
    bool                         m_IsConst;

    /** the member method is noexcept (yes/no) */
    bool                         m_IsNoExcept;

    /** Is anonymous token? (e.g. unnamed struct or union) */
    bool                         m_IsAnonymous;

    /** current Token index in the tree, it is index of the std::vector<Token*>, so use the index,
     *  we can first get a address of the Token, and after a dereference, we can get the Token
     *  instance. Note that the tree is a dynamic object, which means Tokens can be added or removed
     *  from the tree, so the slot of std::vector<Token*> is reused. So, same index does not point
     *  to the same Token if TokenTree are updated or changed.
     */
    int                          m_Index;

    /** Parent Token index */
    int                          m_ParentIndex;

    /** if it is a class kind token, then it contains all the member tokens */
    TokenIdxSet                  m_Children;

    /** all the ancestors in the inheritance hierarchy */
    TokenIdxSet                  m_Ancestors;

    /** the nearest ancestors */
    TokenIdxSet                  m_DirectAncestors;

    /** all the descendants in the inheritance hierarchy */
    TokenIdxSet                  m_Descendants;

    /** used for namespace aliases */
    wxArrayString                m_Aliases;

    /** template argument list, comma separated list string */
    wxString                     m_TemplateArgument;

    /** for a class template, this is the formal template argument list, but for a variable Token,
     *  this is the actual template arguments.
     */
    wxArrayString                m_TemplateType;

    /** a string to string map from formal template argument to actual template argument */
    std::map<wxString, wxString> m_TemplateMap;

    /** alias for templates, e.g. template T1 T2; */
    wxString                     m_TemplateAlias;

    /** custom user-data (the classbrowser expects it to be a pointer to a cbProject), this field
     *  is used when user can only show the tokens belong to the current C::B project in the
     *  browser tree. m_UserData is updated in the worker threaded task: MarkFileAsLocalThreadedTask
     */
    void*                        m_UserData;

protected:

    /** a pointer to TokenTree */
    TokenTree*                   m_TokenTree;

    /** This is used in class browser to avoid duplication nodes in the class browser tree. Once a
     *  Token is allocated from the heap, a new ticket number is assigned to the Token, so if a new
     *  node is only added to the class browser tree if a Token with new ticket is detected.
     */
    size_t                       m_Ticket;
};

#endif // TOKEN_H
