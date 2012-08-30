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
{ fpsNotParsed = 0, fpsAssigned, fpsBeingParsed, fpsDone };

typedef std::deque<int>                                          TokenIdxList;
typedef std::vector<Token*>                                      TokenList;
typedef SearchTree<TokenIdxSet>                                  TokenSearchTree;
typedef BasicSearchTree                                          TokenFilenamesMap;
typedef std::map< size_t, TokenIdxSet,       std::less<size_t> > TokenFilesMap;
typedef std::map< size_t, FileParsingStatus, std::less<size_t> > TokenFilesStatus;

extern wxMutex s_TokensTreeMutex;

class TokensTree
{
    friend class CCDebugInfo;
    friend class CCTest;
    friend class CCTestFrame;
public:

    TokensTree();
    virtual ~TokensTree();

    // STL compatibility functions
    void                  clear();
    inline Token* operator[](int idx)       { return GetTokenAt(idx); }
    inline Token*         at(int idx)       { return GetTokenAt(idx); }
    inline const Token *  at(int idx) const { return GetTokenAt(idx); }
    size_t                size();
    size_t                realsize();
    inline bool           empty()           { return size()==0;       }
    int                   insert(Token* newToken);
    int                   insert(int loc, Token* newToken);
    int                   erase(int loc);
    void                  erase(Token* oldToken);

    inline void Clear()                     { clear();                }

    // Token specific functions
    void   RecalcFreeList();

    // This will convert the Token's ancestor string to it's IDs
    void   RecalcInheritanceChain(Token* token);

    int    TokenExists(const wxString& name, int parent, short int kindMask);
    int    TokenExists(const wxString& name, const wxString& baseArgs, int parent, TokenKind kind);
    size_t FindMatches(const wxString& query, TokenIdxSet& result, bool caseSensitive, bool is_prefix, TokenKind kindMask = tkUndefined);
    size_t FindTokensInFile(const wxString& filename, TokenIdxSet& result, short int kindMask);
    void   RemoveFile(const wxString& filename);
    void   RemoveFile(int fileIndex);

    // Protected access to internal lists / maps
    const TokenList*     GetTokens() const                        { return &m_Tokens;              }
    const TokenIdxSet*   GetGlobalNameSpaces() const              { return &m_GlobalNameSpaces;    }
    const TokenFilesMap* GetFilesMap() const                      { return &m_FilesMap;            }
    const TokenIdxSet*   GetFilesMapByIndex(size_t fileIdx) const
    {
      TokenFilesMap::const_iterator it = m_FilesMap.find(fileIdx);
      return (it == m_FilesMap.end() ? 0 : &(it->second));
    }
    const TokenFilesSet* GetFilesToBeReparsed() const             { return &m_FilesToBeReparsed;   }

    size_t       GetFilesMapSize() const                            { return m_FilesMap.size();             }
    void         InsertFileMapByIndex(size_t fileIdx, int tokenIdx) { m_FilesMap[fileIdx].insert(tokenIdx); }
    void         EraseFileMapByIndex(size_t fileIdx)                { m_FilesMap.erase(fileIdx);            }

    size_t       GetFileStatusCountForIndex(size_t fileIdx) const   { return m_FilesStatus.count(fileIdx);  }
    void         EraseFileStatusByIndex(size_t fileIdx)             { m_FilesStatus.erase(fileIdx);         }

    void         EraseFilesToBeReparsedByIndex(size_t fileIdx)      { m_FilesToBeReparsed.erase(fileIdx);   }

    // Parsing related functions
    size_t         InsertFileOrGetIndex(const wxString& filename);
    size_t         GetFileMatches(const wxString& filename, std::set<size_t>& result, bool caseSensitive, bool is_prefix);
    size_t         GetFileIndex(const wxString& filename);
    const wxString GetFilename(size_t fileIdx) const;
    size_t         ReserveFileForParsing(const wxString& filename, bool preliminary = false);
    void           FlagFileForReparsing(const wxString& filename);
    void           FlagFileAsParsed(const wxString& filename);
    bool           IsFileParsed(const wxString& filename);

    void MarkFileTokensAsLocal(const wxString& filename, bool local = true, void* userData = 0);
    void MarkFileTokensAsLocal(size_t fileIdx, bool local = true, void* userData = 0);

    size_t        m_StructUnionUnnamedCount;
    size_t        m_EnumUnnamedCount;
    size_t        m_TokenTicketCount;

protected:
    Token*        GetTokenAt(int idx);
    Token const * GetTokenAt(int idx) const;
    int           AddToken(Token* newToken, int fileIdx);

    void          RemoveToken(int idx);
    void          RemoveToken(Token* oldToken);

    int           AddTokenToList(Token* newToken, int forceidx);
    void          RemoveTokenFromList(int idx);

    void RecalcFullInheritance(int parentIdx, TokenIdxSet& result); // called by RecalcData

    /** Check all the children belong this token should be removed
      * @param token the checked token pointer
      * @param fileIdx file index the token belongs to
      * @return if true, we can safely remove the token
      */
    bool CheckChildRemove(const Token* token, int fileIdx);

    TokenSearchTree   m_Tree;              /** Tree containing the indexes to the tokens (the indexes will be used on m_Tokens) */
    TokenList         m_Tokens;            /** Contains the pointers to all the tokens */
    TokenIdxList      m_FreeTokens;        /** List of all the deleted (and available) tokens */

    /** List of tokens belonging to the global namespace */
    TokenIdxSet       m_TopNameSpaces;
    TokenIdxSet       m_GlobalNameSpaces;

    TokenFilenamesMap m_FilenamesMap;      /** Map: file names -> file indices */
    TokenFilesMap     m_FilesMap;          /** Map: file indices -> sets of TokenIndexes */
    TokenFilesStatus  m_FilesStatus;       /** Map: file indices -> status */
    TokenFilesSet     m_FilesToBeReparsed; /** Set: file indices */
};

#endif // TOKENSTREE_H
