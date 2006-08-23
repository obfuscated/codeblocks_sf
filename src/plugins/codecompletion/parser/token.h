#ifndef TOKEN_H
#define TOKEN_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/file.h>
#include <wx/thread.h>
#include <wx/stream.h>
#include <settings.h>

#include "blockallocated.h"
#include <globals.h>
#include "searchtree.h"
#include <deque>
using namespace std;

class Token;
class TokensTree;

static wxCriticalSection s_MutexProtection;
enum FileParsingStatus
{
    fpsNotParsed = 0,
    fpsAssigned,
    fpsBeingParsed,
    fpsDone
};

WX_DEFINE_ARRAY(Token*, TokensArray);

typedef vector<Token*> TokenList;

typedef deque<int> TokenIdxList;
typedef set<int, less<int> > TokenIdxSet;
typedef SearchTree<TokenIdxSet> TokenSearchTree;
typedef BasicSearchTree TokenFilenamesMap;
typedef map<size_t,TokenIdxSet,less<size_t> > TokenFilesMap;
typedef map<size_t,FileParsingStatus,less<size_t> > TokenFilesStatus;
typedef set<size_t,less<size_t> > TokenFilesSet;

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


class Token  : public BlockAllocated<Token, 10000>
{
    friend class TokensTree;
	public:
		Token();
		Token(const wxString& name, unsigned int file, unsigned int line);
		~Token();

		void AddChild(int child);
		void RemoveChild(int child);
		wxString GetNamespace() const;
		bool InheritsFrom(int idx) const;
		const wxString DisplayName();
		wxString GetTokenKindString() const;
		wxString GetTokenScopeString() const;
        wxString GetFilename() const;
		wxString GetImplFilename() const;
		bool MatchesFiles(const TokenFilesSet& files);

		bool SerializeIn(wxInputStream* f);
		bool SerializeOut(wxOutputStream* f);
		int GetSelf() { return m_Self; } // current index in the tree
		const wxString GetParentName();
		Token* GetParentToken();
		TokensTree* GetTree() { return m_pTree; }

		wxString m_Type; // this is the return value (if any): e.g. const wxString&
		wxString m_ActualType; // this is what the parser believes is the actual return value: e.g. wxString
		wxString m_Name;
		wxString m_Args;
		wxString m_AncestorsString; // all ancestors comma-separated list
		unsigned int m_File;
		unsigned int m_Line;
		unsigned int m_ImplFile;
		unsigned int m_ImplLine;
		TokenScope m_Scope;
		TokenKind m_TokenKind;
		bool m_IsOperator;
		bool m_IsLocal; // found in a local file?
		bool m_IsTemp; // if true, the tree deletes it in FreeTemporaries()
		bool m_IsTypedef; // typedefs are stored as classes inheriting from the typedef'd type. This flag tells if the token is a typedef.

        int m_ParentIndex;
        TokenIdxSet m_Children;
        TokenIdxSet m_Ancestors;
        TokenIdxSet m_DirectAncestors;
        TokenIdxSet m_Descendants;

		bool m_Bool; // custom bool value
	protected:
        TokensTree* m_pTree;
		int m_Self; // current index in the tree
	private:
};

class TokensTree
{
    public:
        static const wxString s_version;
        TokensTree();
        inline void Clear() { clear(); }

        // STL compatibility functions
        void clear();
        inline Token* operator[](int idx) { return GetTokenAt(idx); }
        inline Token* at(int idx) { return GetTokenAt(idx); }
        size_t size();
        size_t realsize();
        inline bool empty() { return size()==0; }
        int insert(Token* newToken);
        int insert(int loc, Token* newToken);
        int erase(int loc);
        void erase(Token* oldToken);

        // Token specific functions
        void RecalcFreeList();
        void RecalcData();
        int TokenExists(const wxString& name, int parent, short int kindMask);
        size_t FindMatches(const wxString& s,TokenIdxSet& result,bool caseSensitive,bool is_prefix);
        size_t FindTokensInFile(const wxString& file, TokenIdxSet& result, short int kindMask);
        void RemoveFile(const wxString& filename);
        void RemoveFile(int index);
        void FreeTemporaries();
        virtual ~TokensTree();

        // Parsing related functions

        size_t GetFileIndex(const wxString& filename);
        const wxString GetFilename(size_t idx) const;
        size_t ReserveFileForParsing(const wxString& filename,bool preliminary = false);
        void FlagFileForReparsing(const wxString& filename);
        void FlagFileAsParsed(const wxString& filename);
        bool IsFileParsed(const wxString& filename);

        TokenList m_Tokens; /// Contains the pointers to all the tokens
        TokenSearchTree m_Tree; /** Tree containing the indexes to the tokens
          (the indexes will be used on m_Tokens) */

        TokenFilenamesMap m_FilenamesMap; /** Map: filenames -> file indexes */
        TokenFilesMap m_FilesMap; /** Map: file indexes -> Sets of TokenIndexes */
        TokenFilesSet m_FilesToBeReparsed; /** Set: file indexes */
        TokenIdxList m_FreeTokens; /** List of all the deleted (and available) tokens */

        /** List of tokens belonging to the global namespace */
        TokenIdxSet m_TopNameSpaces,
        m_GlobalNameSpace;

        TokenFilesStatus m_FilesStatus; /** Parse Status for each file */
        bool m_modified;
    protected:
        Token* GetTokenAt(int idx);
        int AddToken(Token* newToken,int forceidx = -1);

        void RemoveToken(int idx);
        void RemoveToken(Token* oldToken);

        int AddTokenToList(Token* newToken,int forceidx = -1);
        void RemoveTokenFromList(int idx);

        void RecalcFullInheritance(int parentIdx, TokenIdxSet& result); // called by RecalcData
};


inline void SaveIntToFile(wxOutputStream* f, int i)
{
    /* This used to be done as
        f->Write(&i, sizeof(int));
    which is incorrect because it assumes a consistant byte order
    and a constant int size */

    unsigned int const j = i; // rshifts aren't well-defined for negatives
    unsigned char c[4] = { j>>0&0xFF, j>>8&0xFF, j>>16&0xFF, j>>24&0xFF };
    f->Write( c, 4 );
}

inline bool LoadIntFromFile(wxInputStream* f, int* i)
{
//    See SaveIntToFile
//    return f->Read(i, sizeof(int)) == sizeof(int);

    unsigned char c[4];
    if ( f->Read( c, 4 ).LastRead() != 4 ) return false;
    *i = ( c[0]<<0 | c[1]<<8 | c[2]<<16 | c[3]<<24 );
    return true;
}

inline void SaveStringToFile(wxOutputStream* f, const wxString& str)
{
    const wxWX2MBbuf psz = str.mb_str(wxConvUTF8);
    int size = psz ? strlen(psz) : 0;
    if (size >= 32767)
        size = 32767;
    SaveIntToFile(f, size);
    if(size)
        f->Write(psz, size);
}

inline bool LoadStringFromFile(wxInputStream* f, wxString& str)
{
    int size;
    if (!LoadIntFromFile(f, &size))
        return false;
    bool ok = true;
    if (size > 0 && size <= 32767)
    {
        static char buf[32768];
        ok = f->Read(buf, size).LastRead() == (size_t)size;
        buf[size] = '\0';
        str = wxString(buf, wxConvUTF8);
    }
    else // doesn't fit in our buffer, but still we have to skip it
    {
        str.Empty();
        size = size & 0xFFFFFF; // Can't get any longer than that
        f->SeekI(size, wxFromCurrent);
    }
    return ok;
}

#endif // TOKEN_H
