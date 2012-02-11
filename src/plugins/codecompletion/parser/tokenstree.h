/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENSTREE_H
#define TOKENSTREE_H

#include <wx/string.h>
#include <wx/thread.h>
#include <vector>
#include <deque>

#include "token.h"
#include "searchtree.h"

enum FileParsingStatus
{
    fpsNotParsed = 0,
    fpsAssigned,
    fpsBeingParsed,
    fpsDone
};

typedef std::deque<int>                                          TokenIdxList;
typedef std::vector<Token*>                                      TokenList;
typedef SearchTree<TokenIdxSet>                                  TokenSearchTree;
typedef BasicSearchTree                                          TokenFilenamesMap;
typedef std::map< size_t, TokenIdxSet,       std::less<size_t> > TokenFilesMap;
typedef std::map< size_t, FileParsingStatus, std::less<size_t> > TokenFilesStatus;

extern wxMutex s_TokensTreeMutex;

class TokensTree
{
public:
    TokensTree();
    virtual ~TokensTree();

    inline void Clear()                    { clear(); }

    // STL compatibility functions
    void          clear();
    inline Token* operator[](int idx)      { return GetTokenAt(idx); }
    inline Token* at(int idx)              { return GetTokenAt(idx); }
    inline const Token * at(int idx) const { return GetTokenAt(idx); }
    size_t        size();
    size_t        realsize();
    inline bool   empty()                  { return size()==0; }
    int           insert(Token* newToken);
    int           insert(int loc, Token* newToken);
    int           erase(int loc);
    void          erase(Token* oldToken);

    // Token specific functions
    void   RecalcFreeList();
    void   RecalcInheritanceChain(Token* token);
    int    TokenExists(const wxString& name, int parent, short int kindMask);
    int    TokenExists(const wxString& name, const wxString& baseArgs, int parent, TokenKind kind);
    size_t FindMatches(const wxString& s, TokenIdxSet& result, bool caseSensitive, bool is_prefix, short int kindMask = tkUndefined);
    size_t FindTokensInFile(const wxString& file, TokenIdxSet& result, short int kindMask);
    void   RemoveFile(const wxString& filename);
    void   RemoveFile(int fileIndex);

    // Parsing related functions
    size_t         GetFileIndex(const wxString& filename);
    const wxString GetFilename(size_t idx) const;
    size_t         ReserveFileForParsing(const wxString& filename, bool preliminary = false);
    void           FlagFileForReparsing(const wxString& filename);
    void           FlagFileAsParsed(const wxString& filename);
    bool           IsFileParsed(const wxString& filename);

    void MarkFileTokensAsLocal(const wxString& filename, bool local = true, void* userData = 0);
    void MarkFileTokensAsLocal(size_t file, bool local = true, void* userData = 0);

    TokenList         m_Tokens;            /** Contains the pointers to all the tokens */
    TokenSearchTree   m_Tree;              /** Tree containing the indexes to the tokens (the indexes will be used on m_Tokens) */

    TokenFilenamesMap m_FilenamesMap;      /** Map: filenames    -> file indexes */
    TokenFilesMap     m_FilesMap;          /** Map: file indexes -> sets of TokenIndexes */
    TokenFilesSet     m_FilesToBeReparsed; /** Set: file indexes */
    TokenIdxList      m_FreeTokens;        /** List of all the deleted (and available) tokens */

    /** List of tokens belonging to the global namespace */
    TokenIdxSet       m_TopNameSpaces;
    TokenIdxSet       m_GlobalNameSpace;

    TokenFilesStatus  m_FilesStatus;       /** Parse status for each file */
    bool              m_Modified;
    size_t            m_StructUnionUnnamedCount;
    size_t            m_EnumUnnamedCount;
    size_t            m_TokenTicketCount;

protected:
    Token*        GetTokenAt(int idx);
    Token const * GetTokenAt(int idx) const;
    int           AddToken(Token* newToken, int fileIndex);

    void RemoveToken(int idx);
    void RemoveToken(Token* oldToken);

    int  AddTokenToList(Token* newToken, int forceidx);
    void RemoveTokenFromList(int idx);

    void RecalcFullInheritance(int parentIdx, TokenIdxSet& result); // called by RecalcData

    /** Check all the children belong this token should be remove
      * @param token the checked token pointer
      * @param fileIndex file index the token belongs to
      * @return if true, we can safely remove the token
      */
    bool CheckChildRemove(Token * token, int fileIndex);
};

#endif // TOKENSTREE_H
