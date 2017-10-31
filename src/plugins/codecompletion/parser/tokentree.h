/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENTREE_H
#define TOKENTREE_H

#include <wx/string.h>
#include <wx/thread.h>

#include <vector>
#include <deque>

#include "token.h"
#include "searchtree.h"

enum FileParsingStatus
{ fpsNotParsed = 0, fpsAssigned, fpsBeingParsed, fpsDone };

typedef std::deque<int>                                          TokenIdxList;
typedef std::vector<Token*>                                      TokenList;
typedef SearchTree<TokenIdxSet>                                  TokenSearchTree;
typedef BasicSearchTree                                          TokenFilenameMap;
typedef std::map< size_t, TokenIdxSet,       std::less<size_t> > TokenFileMap;
typedef std::map< size_t, FileParsingStatus, std::less<size_t> > TokenFileStatusMap;
typedef std::map< int, wxString >                                TokenIdxStringMap;

extern wxMutex s_TokenTreeMutex;

/** a container class to hold all the Tokens getting from parsing stage
 *
 *  To query a Token, a unique index is needed, the index is actually position in the
 *  std::vector<Token*> (namely TokenList).
 *
 */
class TokenTree
{
    friend class CCDebugInfo;
    friend class CCTest;
    friend class CCTestFrame;
    friend class NativeParserTest;
public:

    TokenTree();
    virtual ~TokenTree();

    // STL compatibility functions
    void                  clear();
    inline Token* operator[](int idx)       { return GetTokenAt(idx); }
    inline Token*         at(int idx)       { return GetTokenAt(idx); }
    inline const Token *  at(int idx) const { return GetTokenAt(idx); }

    /** total size of std::vector<Token*> */
    size_t                size();

    /** some position of the std::vector<Token*> are not used, so the real size maybe a bit smaller
     *  than the total size
     */
    size_t                realsize();

    /** check to see whether the TokenTree is empty*/
    inline bool           empty()           { return size()==0;       }

    /** add a new Token instance to the TokenTree
     * @param newToken the pointer to a Token instance
     * @return the index of the Token in the TokenTree
     */
    int                   insert(Token* newToken);

    /** add a new Token instance to the TokenTree
     * @param loc -1 means we add a new slot to the Token list (vector), otherwise, the new added
     * Token will replace the old one in that location(index).
     */
    int                   insert(int loc, Token* newToken);

    /** remove the Token specified by the index */
    int                   erase(int loc);

    /** remove the Token from the TokenTree
     *  @param oldToken the token need to be removed, if success, the pointer is invalid after the
     *  function call
     */
    void                  erase(Token* oldToken);

    inline void Clear()                     { clear();                }

    // Token specific functions
    /** collect the unused slots in the std::vector<Token*> */
    void   RecalcFreeList();

    /** only the token's name need to be changed, thus update the index map of the TokenTree */
    void   RenameToken(Token* token, const wxString& newName);

    /** convert the Token's ancestor string to it's IDs */
    void   RecalcInheritanceChain(Token* token);

    /** @brief query tokens by names
     *
     * @param name the search key, token's name
     * @param parent search only the tokens under the token specified by parent
     * @param kindMask token kind mask
     * @return int the first matched token index will be returned, return -1 if no matched token is
     * found.
     */
    int    TokenExists(const wxString& name, int parent, short int kindMask);

    /** @brief query tokens by names
     *
     * @param name the search key, token's name
     * @param baseArgs specify the function parameter
     * @param parent search only the tokens under the token specified by parent
     * @param kindMask token kind mask
     * @return int the first matched token index will be returned, return -1 if no matched token is
     * found.
     */
    int    TokenExists(const wxString& name, const wxString& baseArgs, int parent, TokenKind kind);

    /** @brief query tokens by names
     *
     * @param name the search key, token's name
     * @param parents search only the tokens under the token specified by parent collection tokens
     * @param kindMask token kind mask
     * @return int the first matched token index will be returned, return -1 if no matched token is
     * found.
     */
    int    TokenExists(const wxString& name, const TokenIdxSet& parents, short int kindMask);

    /** @brief query tokens by names
     *
     * @param name the search key, token's name
     * @param baseArgs specify the function parameter
     * @param parents search only the tokens under the token specified by parent collection tokens
     * @param kindMask token kind mask
     * @return int the first matched token index will be returned, return -1 if no matched token is
     * found.
     */
    int    TokenExists(const wxString& name, const wxString& baseArgs, const TokenIdxSet& parents,
                       TokenKind kind);

    /** @brief find a collection of matched tokens
     *
     * @param query search name
     * @param result the return collection token index
     * @param caseSensitive true if we need the case sensitive search for the query name
     * @param is_prefix true if the query is only a prefix of any matched token
     * @param kindMask token kind match mask
     * @return the number of matched tokens
     */
    size_t FindMatches(const wxString& query, TokenIdxSet& result, bool caseSensitive,
                       bool is_prefix, TokenKind kindMask = tkUndefined);


    /** @brief find tokens belong to a specified file
     *
     * @param filename search key
     * @param result returned tokens
     * @param kindMask token filter mask
     * @return the number of matched tokens
     *
     */
    size_t FindTokensInFile(const wxString& filename, TokenIdxSet& result, short int kindMask);

    /** remove tokens belong to the file */
    void   RemoveFile(const wxString& filename);

    /** remove tokens belong to the file */
    void   RemoveFile(int fileIndex);

    // Protected access to internal lists / maps
    const TokenList*     GetTokens() const
    {
        return &m_Tokens;
    }

    const TokenIdxSet*   GetGlobalNameSpaces() const
    {
        return &m_GlobalNameSpaces;
    }

    const TokenFileMap*  GetFilesMap() const
    {
        return &m_FileMap;
    }

    const TokenIdxSet*   GetTokensBelongToFile(size_t fileIdx) const
    {
      TokenFileMap::const_iterator it = m_FileMap.find(fileIdx);
      return (it == m_FileMap.end() ? 0 : &(it->second));
    }

    const TokenFileSet* GetFilesToBeReparsed() const
    {
        return &m_FilesToBeReparsed;
    }

    size_t       GetFileMapSize() const
    {
        return m_FileMap.size();
    }

    void         InsertTokenBelongToFile(size_t fileIdx, int tokenIdx)
    {
        m_FileMap[fileIdx].insert(tokenIdx);
    }

    void         EraseFileMapInFileMap(size_t fileIdx)
    {
        m_FileMap.erase(fileIdx);
    }

    size_t       GetFileStatusCountForIndex(size_t fileIdx) const
    {
        return m_FileStatusMap.count(fileIdx);
    }

    void         EraseFileStatusByIndex(size_t fileIdx)
    {
        m_FileStatusMap.erase(fileIdx);
    }

    void         EraseFilesToBeReparsedByIndex(size_t fileIdx)
    {
        m_FilesToBeReparsed.erase(fileIdx);
    }

    // Parsing related functions
    /** put the filename in the m_FilenameMap, and return the file index, if this file is already in
     * the m_FilenameMap, it just return the file index.
     */
    size_t         InsertFileOrGetIndex(const wxString& filename);

    size_t         GetFileMatches(const wxString& filename, std::set<size_t>& result,
                                  bool caseSensitive, bool is_prefix);

    size_t         GetFileIndex(const wxString& filename);

    const wxString GetFilename(size_t fileIdx) const;

    /** mark a file to be parsed. Or, assigned, return non-zero if success.
     * @param filename the file need to be parsed
     * @param preliminary if true, this means we will put the file status as assigned (not parse it
     * soon, just assigned a Parserthread task, and the actually parsing will be done later; if
     * false, then set the file status to being parsed, so parsing must be happened immediately after
     * this function call.
     */
    size_t         ReserveFileForParsing(const wxString& filename, bool preliminary = false);

    /** mark the file as "need to be reparsed" status, usually happens that this file is saved(updated)
     * so a reparse need to be done.
     */
    void           FlagFileForReparsing(const wxString& filename);

    /** mark the file status as fpsDone, since parsing this file is done */
    void           FlagFileAsParsed(const wxString& filename);

    /** is the file name is in the tokentree, and it's status is either assigned or beingparsed or done
     * also, should make sure that this file is not marked as "need to be reparsed".
     */
    bool           IsFileParsed(const wxString& filename);

    /** @brief mark the tokens so that they are associated with a C::B project
     *
     * @param filename specify the tokens which belong to the file
     * @param local true if the tokens belong to project files
     * @param userData a pointer to the c::b project
     */
    void MarkFileTokensAsLocal(const wxString& filename, bool local = true, void* userData = 0);

    /** @brief mark the tokens so that they are associated with a C::B project
     *
     * @param fileIdx specify the tokens which belong to the file
     * @param local true if the tokens belong to project files
     * @param userData a pointer to the c::b project
     */
    void MarkFileTokensAsLocal(size_t fileIdx, bool local = true, void* userData = 0);

    // Documentation related functions

    /** @brief associate a document string with the token
     *
     * @param tokenIdx token index
     * @param fileIdx the file index where the doc locates
     * @param doc document string
     */
    void AppendDocumentation(int tokenIdx, unsigned int fileIdx, const wxString& doc);

    /** @brief get the document string associated with the token
     *
     * @param tokenIdx token index
     * @return wxString the document
     */
    wxString GetDocumentation(int tokenIdx);

    size_t        m_TokenTicketCount;

protected:
    Token*        GetTokenAt(int idx);
    Token const * GetTokenAt(int idx) const;
    int           AddToken(Token* newToken, int forceidx = -1);

    void          RemoveToken(int idx);
    void          RemoveToken(Token* oldToken);

    /** add the Token pointer to the vector<Token*>, mostly the default value forceidx = -1 is used
     *  which add a new slot in the vector or reused an empty slot. if forceidx >= 0, this means
     *  we need to replace the value in the specified slot index, the later case only happens we are
     *  re-construct the Tokentree from the cache.
     *  @ret always return the used slot index in the vector.
     */
    int           AddTokenToList(Token* newToken, int forceidx = -1);
    /** Remove the Token specified by the idx in the vector<Token*>, note the Token instance is
     *  destroyed, and the slot becomes empty. The empty slot was recored and will be re-used later.
     */
    void          RemoveTokenFromList(int idx);

    void RecalcFullInheritance(int parentIdx, TokenIdxSet& result); // called by RecalcData

    /** Check all the children belong this token should be removed
      * @param token the checked token pointer
      * @param fileIdx file index the token belongs to
      * @return if true, we can safely remove the token
      */
    bool CheckChildRemove(const Token* token, int fileIdx);

    /** This is a string->TokenIndexSet map. E.g. we have a class Token named "AAA", also, we can
     *  have a function Token named "AAA", they are  different Tokens, but they share the same name.
     *  So we may have an tree point "AAA" -> <30, 40> map in the TokenSearchTree. Note here 30 and
     *  40 are the indexes (slots) in the m_Tokens vector, which holds the Tokens pointers. In-fact,
     * "AAA" -> <30, 40> is not mapped directly, it has a indirect map like "AAA" -> 16 -> <30, 40>,
     *  the middle number 16 here is the tree point item index in the BasicSearchTree, as we have
     *  a std::vector< TokenIndexSet > in the SearchTree, the 16 is the index to fetch the value
     *  <30,40> in the vector
     */
	TokenSearchTree   m_Tree;

    /** Contains the pointers to all the Token instances, it is just a std::vector<Token*>, the
     *  suggest way to access a Token instance is by first get its index in the m_Tokens, then get
     *  its address by m_Tokens[index], the reason we have such indirect access is that there are
     *  many Tokens which can reference each other, it is much safe using index instead of raw
     *  pointers.
     */
	TokenList         m_Tokens;

	/** List of all the deleted (and available) tokens. When more and more Tokens were allocated,
	 *  their address was recorded in the m_Tokens, m_Tokens grows larger, but if we delete some
	 *  Tokens, the will have some empty slots in the std::vector<Token*>, we need to reuse those
	 *  slots, here m_FreeTokens is to hold those empty slots for reuse, so we don't waste empty
	 *  slots in the m_Tokens */
	TokenIdxList      m_FreeTokens;

    /** List of tokens belonging to the global namespace */
    TokenIdxSet       m_TopNameSpaces;
    TokenIdxSet       m_GlobalNameSpaces;

    /** Map: file names -> file indices */
    TokenFilenameMap    m_FilenameMap;

    /** Map: file indices -> sets of TokenIndexes */
    TokenFileMap        m_FileMap;

    /** Map: file indices -> status */
    TokenFileStatusMap  m_FileStatusMap;

    /** Set: file indices */
    TokenFileSet        m_FilesToBeReparsed;
};

#endif // TOKENTREE_H
