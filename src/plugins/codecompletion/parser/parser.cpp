/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include <wx/log.h>
#include <wx/app.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/progdlg.h>
#include "parser.h"
#ifndef STANDALONE
	#include <configmanager.h>
	#include <messagemanager.h>
	#include <manager.h>
	#include <globals.h>
#endif // STANDALONE

static wxMutex s_mutexListProtection;
int PARSER_END = wxNewId();
static int idPool = wxNewId();

BEGIN_EVENT_TABLE(Parser, wxEvtHandler)
//	EVT_MENU(NEW_TOKEN, Parser::OnNewToken)
//	EVT_MENU(FILE_NEEDS_PARSING, Parser::OnParseFile)
	EVT_THREADTASK_STARTED(idPool, Parser::OnStartThread)
	EVT_THREADTASK_ENDED(idPool, Parser::OnEndThread)
    EVT_THREADTASK_ALLDONE(idPool, Parser::OnAllThreadsDone)
END_EVENT_TABLE()

Parser::Parser(wxEvtHandler* parent)
	: m_MaxThreadsCount(8),
	m_pParent(parent)
#ifndef STANDALONE
	,m_pImageList(0L)
#endif
    ,m_abort_flag(false),
    m_UsingCache(false),
    m_CacheFilesCount(0),
    m_CacheTokensCount(0),
    m_Pool(this, idPool)
{
	ReadOptions();
#ifndef STANDALONE
	m_pImageList = new wxImageList(16, 16);
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::Get()->Read(_T("data_path")) + _T("/images/codecompletion/");
    // bitmaps must be added by order of PARSER_IMG_* consts
    bmp.LoadFile(prefix + _T("class_folder.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CLASS_FOLDER
    bmp.LoadFile(prefix + _T("class.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CLASS
    bmp.LoadFile(prefix + _T("ctor_private.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PRIVATE
    bmp.LoadFile(prefix + _T("ctor_protected.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PROTECTED
    bmp.LoadFile(prefix + _T("ctor_public.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PUBLIC
    bmp.LoadFile(prefix + _T("dtor_private.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PRIVATE
    bmp.LoadFile(prefix + _T("dtor_protected.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PROTECTED
    bmp.LoadFile(prefix + _T("dtor_public.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PUBLIC
    bmp.LoadFile(prefix + _T("method_private.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp.LoadFile(prefix + _T("method_protected.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp.LoadFile(prefix + _T("method_public.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PUBLIC
    bmp.LoadFile(prefix + _T("var_private.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PRIVATE
    bmp.LoadFile(prefix + _T("var_protected.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PROTECTED
    bmp.LoadFile(prefix + _T("var_public.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PUBLIC
    bmp.LoadFile(prefix + _T("preproc.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_PREPROCESSOR
    bmp.LoadFile(prefix + _T("enum.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUM
    bmp.LoadFile(prefix + _T("enumerator.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUMERATOR
    bmp.LoadFile(prefix + _T("namespace.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_NAMESPACE
    bmp.LoadFile(prefix + _T("symbols_folder.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_SYMBOLS_FOLDER
    bmp.LoadFile(prefix + _T("enums_folder.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUMS_FOLDER
    bmp.LoadFile(prefix + _T("preproc_folder.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_PREPROC_FOLDER
    bmp.LoadFile(prefix + _T("others_folder.png"), wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_OTHERS_FOLDER
#endif // STANDALONE
    ConnectEvents();
}

Parser::~Parser()
{
    DisconnectEvents();
	Clear();
#ifndef STANDALONE
	delete m_pImageList;
#endif // STANDALONE
}

void Parser::ConnectEvents()
{
//    Connect(EVT_THREADTASK_STARTED, -1, cbEVT_THREADTASK_STARTED,
//            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
//            &Parser::OnStartThread);
//    Connect(EVT_THREADTASK_ENDED, -1, cbEVT_THREADTASK_ENDED,
//            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
//            &Parser::OnEndThread);
    Connect(NEW_TOKEN, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnNewToken);
    Connect(FILE_NEEDS_PARSING, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnParseFile);
}

void Parser::DisconnectEvents()
{
//    Disconnect(EVT_THREADTASK_STARTED, -1, cbEVT_THREADTASK_STARTED);//,
//    Disconnect(EVT_THREADTASK_ENDED, -1, cbEVT_THREADTASK_ENDED);//,
    Disconnect(NEW_TOKEN, -1, wxEVT_COMMAND_MENU_SELECTED);//,
    Disconnect(FILE_NEEDS_PARSING, -1, wxEVT_COMMAND_MENU_SELECTED);//,
}

void Parser::ReadOptions()
{
#ifdef STANDALONE
	m_Options.followLocalIncludes = true;
	m_Options.followGlobalIncludes = false;
	m_Options.caseSensitive = true;
	m_Options.wantPreprocessor = false;
	m_Options.useSmartSense = true;
	m_BrowserOptions.showInheritance = false;
	m_BrowserOptions.viewFlat = false;
#else // !STANDALONE
	m_MaxThreadsCount = ConfigManager::Get()->Read(_T("/code_completion/max_threads"), 8);
	m_Options.followLocalIncludes = ConfigManager::Get()->Read(_T("/code_completion/parser_follow_local_includes"), 1L);
	m_Options.followGlobalIncludes = ConfigManager::Get()->Read(_T("/code_completion/parser_follow_global_includes"), 0L);
	m_Options.caseSensitive = ConfigManager::Get()->Read(_T("/code_completion/case_sensitive"), 0L);
	m_Options.useSmartSense = ConfigManager::Get()->Read(_T("/code_completion/use_SmartSense"), 1);
	m_Options.wantPreprocessor = ConfigManager::Get()->Read(_T("/code_completion/want_preprocessor"), 0L);
	m_BrowserOptions.showInheritance = ConfigManager::Get()->Read(_T("/code_completion/browser_show_inheritance"), 0L);
	m_BrowserOptions.viewFlat = ConfigManager::Get()->Read(_T("/code_completion/browser_view_flat"), 0L);
#endif // STANDALONE
}

void Parser::WriteOptions()
{
#ifndef STANDALONE
	ConfigManager::Get()->Write(_T("/code_completion/max_threads"), (int)m_MaxThreadsCount);
	ConfigManager::Get()->Write(_T("/code_completion/parser_follow_local_includes"), m_Options.followLocalIncludes);
	ConfigManager::Get()->Write(_T("/code_completion/parser_follow_global_includes"), m_Options.followGlobalIncludes);
	ConfigManager::Get()->Write(_T("/code_completion/case_sensitive"), m_Options.caseSensitive);
	ConfigManager::Get()->Write(_T("/code_completion/use_SmartSense"), m_Options.useSmartSense);
	ConfigManager::Get()->Write(_T("/code_completion/want_preprocessor"), m_Options.wantPreprocessor);
	ConfigManager::Get()->Write(_T("/code_completion/browser_show_inheritance"), m_BrowserOptions.showInheritance);
	ConfigManager::Get()->Write(_T("/code_completion/browser_view_flat"), m_BrowserOptions.viewFlat);
#endif // STANDALONE
}

#define TOKEN_REC 0xFFFFFFFE
#define FILE_REC 0xFFFFFFFD

bool Parser::ReadFromCache(wxFile* f)
{
    Clear();

    unsigned int counter = 0;
    unsigned int length = f->Length();
    wxProgressDialog progress(_("Code-completion plugin"), _("Please wait while loading code-completion cache..."), length);

    // m_Tokens
    while (!f->Eof())
    {
        int rec;
        if (!LoadIntFromFile(f, &rec) || rec != (int)TOKEN_REC)
            break;
        if (!LoadTokenFromCache(f, 0))
            break;
        progress.Update(f->Tell());
    }

    // m_ParsedFiles
    while (!f->Eof())
    {
        wxString file;
        if (!LoadStringFromFile(f, file))
            break;
        m_ParsedFiles.Add(file);
        progress.Update(f->Tell());
    }

    LinkInheritance(); // fix ancestors relationships

    m_UsingCache = true;
    m_CacheFilesCount = m_ParsedFiles.GetCount();
    m_CacheTokensCount = m_Tokens.GetCount();

    return true;
}

bool Parser::CacheNeedsUpdate()
{
    if (m_UsingCache)
    {
        if (m_CacheFilesCount == (int)m_ParsedFiles.GetCount() &&
            m_CacheTokensCount == (int)m_Tokens.GetCount())
        {
            // in-mem data and cache seem to be in sync
            // @warning: this is *not* bulletproof!
            // consider a token name change, for example...
            // maybe use a CRC of some kind?
            return false;
        }
    }
    return true;
}

bool Parser::WriteToCache(wxFile* f)
{
    unsigned int tcount = m_Tokens.GetCount();
    unsigned int fcount = m_ParsedFiles.GetCount();
    unsigned int counter = 0;

    wxProgressDialog progress(_("Code-completion plugin"), _("Please wait while saving code-completion cache..."), tcount + fcount);

    // m_Tokens
    for (unsigned int i = 0; i < tcount; ++i)
    {
        Token* token = m_Tokens[i];
        // only save globals (it's recursive)
        if (!token->m_pParent && !token->m_IsTemporary)
        {
            SaveIntToFile(f, TOKEN_REC);
            SaveTokenToCache(f, token);
        }
        progress.Update(++counter);
    }

    SaveIntToFile(f, FILE_REC);

    // m_ParsedFiles
    for (unsigned int i = 0; i < fcount; ++i)
    {
        SaveStringToFile(f, m_ParsedFiles[i]);
        progress.Update(++counter);
    }

    return true;
}

Token* Parser::LoadTokenFromCache(wxFile* f, Token* parent)
{
//    int rec;
//    if (!LoadIntFromFile(f, &rec) || rec != (int)TOKEN_REC)
//        return 0;

    Token* token = new Token;
    token->m_pParent = parent;

    bool ok = false;
    // this loop actually runs only once
    // it is a block that if we break before ok==true, the token is considered
    // invalid and deleted.
    // it is a simple way to avoid goto's and stuff...
    while (true)
    {
        if (!LoadStringFromFile(f, token->m_Type)) break;
        if (!LoadStringFromFile(f, token->m_ActualType)) break;
        if (!LoadStringFromFile(f, token->m_Name)) break;
        if (!LoadStringFromFile(f, token->m_DisplayName)) break;
        if (!LoadStringFromFile(f, token->m_Args)) break;
        if (!LoadStringFromFile(f, token->m_AncestorsString)) break;
        if (!LoadStringFromFile(f, token->m_Filename)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_Line)) break;
        if (!LoadStringFromFile(f, token->m_ImplFilename)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_ImplLine)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_Scope)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_TokenKind)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_IsOperator)) break;
        if (!LoadIntFromFile(f, (int*)&token->m_IsLocal)) break;

        ok = true;
        m_Tokens.Add(token);
        if (parent)
            parent->m_Children.Add(token);

        int ccount;
        if (!LoadIntFromFile(f, &ccount)) break;

        for (int i = 0; i < ccount; ++i)
            LoadTokenFromCache(f, token);

        break; // no looping
    }

    if (!ok)
    {
        delete token;
        token = 0;
    }

    return token;
}

void Parser::SaveTokenToCache(wxFile* f, Token* token)
{
//    SaveIntToFile(f, TOKEN_REC);

    SaveStringToFile(f, token->m_Type);
    SaveStringToFile(f, token->m_ActualType);
    SaveStringToFile(f, token->m_Name);
    SaveStringToFile(f, token->m_DisplayName);
    SaveStringToFile(f, token->m_Args);
    SaveStringToFile(f, token->m_AncestorsString);
    SaveStringToFile(f, token->m_Filename);
    SaveIntToFile(f, token->m_Line);
    SaveStringToFile(f, token->m_ImplFilename);
    SaveIntToFile(f, token->m_ImplLine);
    SaveIntToFile(f, token->m_Scope);
    SaveIntToFile(f, token->m_TokenKind);
    SaveIntToFile(f, token->m_IsOperator);
    SaveIntToFile(f, token->m_IsLocal);

    int tcount = (int)token->m_Children.GetCount();
    SaveIntToFile(f, tcount); // save num of children

    for (int i = 0; i < tcount; ++i)
    {
        Token* tok = token->m_Children[i];
        SaveTokenToCache(f, tok);
    }
}

inline void Parser::SaveStringToFile(wxFile* f, const wxString& str)
{
    const wxWX2MBbuf psz = str.mb_str(wxConvUTF8);
    int size = psz ? strlen(psz) : 0;
    if (size >= 512)
        size = 512;
    SaveIntToFile(f, size);
    if(size)
        f->Write(psz, size);
}

inline void Parser::SaveIntToFile(wxFile* f, int i)
{
    /* This used to be done as
        f->Write(&i, sizeof(int));
    which is incorrect because it assumes a consistant byte order
    and a constant int size */

    unsigned int const j = i; // rshifts aren't well-defined for negatives
    unsigned char c[4] = { j>>0&0xFF, j>>8&0xFF, j>>16&0xFF, j>>24&0xFF };
    f->Write( c, 4 );
}

inline bool Parser::LoadStringFromFile(wxFile* f, wxString& str)
{
    int size;
    if (!LoadIntFromFile(f, &size))
        return false;
    bool ok = true;
    if (size > 0 && size <= 512)
    {
        static char buf[513];
        ok = f->Read(buf, size) == size;
        buf[size] = '\0';
        str = _U(buf);
    }
    else // doesn't fit in our buffer, but still we have to skip it
    {
        str.Empty();
        size = size & 0xFFFFFF; // Can't get any longer than that
        f->Seek(size, wxFromCurrent);
    }
    return ok;
}

inline bool Parser::LoadIntFromFile(wxFile* f, int* i)
{
//    See SaveIntToFile
//    return f->Read(i, sizeof(int)) == sizeof(int);

    unsigned char c[4];
    if ( f->Read( c, 4 ) != 4 ) return false;
    *i = ( c[0]<<0 | c[1]<<8 | c[2]<<16 | c[3]<<24 );
    return true;
}

unsigned int Parser::GetFilesCount()
{
	wxMutexLocker lock(s_mutexListProtection);
	return m_ParsedFiles.GetCount();
}

bool Parser::Done()
{
    wxMutexLocker lock(s_mutexListProtection);
	return m_Pool.Done();
}

#ifndef STANDALONE
void Parser::SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask)
{
	if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
		return;
#ifdef __WXMSW__
	m_pImageList->Replace(kind, bitmap, mask);
#endif
}

void Parser::SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColor)
{
	if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
		return;
	m_pImageList->Replace(kind, bitmap);//, maskColor);
}

void Parser::SetTokenKindImage(int kind, const wxIcon& icon)
{
	if (kind < PARSER_IMG_MIN || kind > PARSER_IMG_MAX)
		return;
	m_pImageList->Replace(kind, icon);
}

int Parser::GetTokenKindImage(Token* token)
{
    if (!token)
        return PARSER_IMG_NONE;

	switch (token->m_TokenKind)
	{
		case tkPreprocessor: return PARSER_IMG_PREPROCESSOR;

		case tkEnum: return PARSER_IMG_ENUM;

		case tkEnumerator: return PARSER_IMG_ENUMERATOR;

		case tkClass: return PARSER_IMG_CLASS;

		case tkNamespace: return PARSER_IMG_NAMESPACE;

		case tkConstructor:
			switch (token->m_Scope)
			{
				case tsProtected: return PARSER_IMG_CTOR_PROTECTED;
				case tsPrivate: return PARSER_IMG_CTOR_PRIVATE;
				default: return PARSER_IMG_CTOR_PUBLIC;
			}

		case tkDestructor:
			switch (token->m_Scope)
			{
				case tsProtected: return PARSER_IMG_DTOR_PROTECTED;
				case tsPrivate: return PARSER_IMG_DTOR_PRIVATE;
				default: return PARSER_IMG_DTOR_PUBLIC;
			}

		case tkFunction:
			switch (token->m_Scope)
			{
				case tsProtected: return PARSER_IMG_FUNC_PROTECTED;
				case tsPrivate: return PARSER_IMG_FUNC_PRIVATE;
				default: return PARSER_IMG_FUNC_PUBLIC;
			}

		case tkVariable:
			switch (token->m_Scope)
			{
				case tsProtected: return PARSER_IMG_VAR_PROTECTED;
				case tsPrivate: return PARSER_IMG_VAR_PRIVATE;
				default: return PARSER_IMG_VAR_PUBLIC;
			}

        default: return PARSER_IMG_NONE;
    }
}
#endif // STANDALONE

Token* Parser::FindTokenByName(const wxString& name, bool globalsOnly, short int kindMask)
{
//	for (unsigned int i = m_Tokens.GetCount() - 1; i >= 0; --i)
    Token* res = 0;
	for (unsigned int i = 0; i < m_Tokens.GetCount(); ++i)
	{
		Token* token = m_Tokens[i];
		if (globalsOnly && token->m_pParent)
			continue;
		if ((token->m_TokenKind & kindMask) && token->m_Name.Matches(name))
		{
            res = token;
//            Manager::Get()->GetMessageManager()->DebugLog("token=%s (%d)", name.c_str(), token->m_Children.GetCount());
//            return token;
        }
	}
//	return 0;
	return res;
}

Token* Parser::FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance, short int kindMask)
{
	if (!parent)
		return FindTokenByName(name, false, kindMask);

	for (unsigned int i = 0; i < parent->m_Children.GetCount(); ++i)
	{
		Token* token = parent->m_Children[i];
		if ((token->m_TokenKind & kindMask) && token->m_Name.Matches(name))
			return token;
	}
	// not found; check ancestors now...
	if (useInheritance)
	{
		for (unsigned int i = 0; i < parent->m_Ancestors.GetCount(); ++i)
		{
			Token* inherited = FindChildTokenByName(parent->m_Ancestors[i], name, true, kindMask);
			if (inherited)
				return inherited;
		}
	}
	return 0L;
}

Token* Parser::FindTokenByDisplayName(const wxString& name)
{
	for (unsigned int i = 0; i < m_Tokens.GetCount(); ++i)
	{
		Token* token = m_Tokens[i];
		if (token->m_DisplayName.Matches(name))
			return token;
	}
	return 0L;
}

int TokensSortProc(Token** first, Token** second)
{
	Token* parent1 = first[0]->m_pParent;
	Token* parent2 = second[0]->m_pParent;
	int diff = 0;
	if (first[0]->m_IsTemporary != second[0]->m_IsTemporary)
	{
		// local block token, always first in list
		return first[0]->m_IsTemporary ? -1 : 1;
	}
	else if (first[0]->m_IsLocal != second[0]->m_IsLocal)
	{
		// project tokens first, then global
		return first[0]->m_IsLocal ? -1 : 1;
	}
	else if (parent1 && !parent2)
	{
		// first tokens that have a parent
		return -1;
	}
	else if (!parent1 && parent2)
	{
		// first tokens that have a parent
		return 1;
	}
	else if (parent1 && parent2)
	{
		if (parent1 != parent2)
		{
			// if both tokens have parent, order by *parent* name
			diff = parent1->m_Name.CompareTo(parent2->m_Name);
			if (diff)
				return diff;
		}
	}
	// order by token kind
	int ret = first[0]->m_TokenKind - second[0]->m_TokenKind;
	// finally order by token name, if all else fails...
	if (!ret)
		ret = first[0]->m_Name.CompareTo(second[0]->m_Name);
	return ret;
}

void Parser::SortAllTokens()
{
	m_Tokens.Sort(TokensSortProc);
}

void Parser::LinkInheritance(bool tempsOnly)
{
	//Manager::Get()->GetMessageManager()->DebugLog("Linking inheritance...");
	for (unsigned int i = 0; i < m_Tokens.GetCount(); ++i)
	{
		Token* token = m_Tokens[i];

		if (token->m_TokenKind != tkClass)
			continue;

		if (tempsOnly && !token->m_IsTemporary)
			continue;

		if (token->m_AncestorsString.IsEmpty())
			continue;

		// only local symbols might change inheritance
		if (token->m_IsLocal)
		{
			//Manager::Get()->GetMessageManager()->DebugLog("Removing ancestors from %s", token->m_Name.c_str());
			token->m_Ancestors.Clear();
		}
		else
			continue;

		//Manager::Get()->GetMessageManager()->DebugLog("Token %s, Ancestors %s", token->m_Name.c_str(), token->m_AncestorsString.c_str());
		wxStringTokenizer tkz(token->m_AncestorsString, _T(","));
		while (tkz.HasMoreTokens())
		{
			wxString ancestor = tkz.GetNextToken();
			if (ancestor.IsEmpty() || ancestor == token->m_Name)
				continue;
			//Manager::Get()->GetMessageManager()->DebugLog("Ancestor %s", ancestor.c_str());
			Token* ancestorToken = FindTokenByName(ancestor, tkClass);
			//Manager::Get()->GetMessageManager()->DebugLog(ancestorToken ? "Found" : "not Found");
			if (ancestorToken)
			{
				//Manager::Get()->GetMessageManager()->DebugLog("Adding ancestor %s to %s", ancestorToken->m_Name.c_str(), token->m_Name.c_str());
				token->m_Ancestors.Add(ancestorToken);
			}
		}
		if (!token->m_IsLocal) // global symbols are linked once
		{
			//Manager::Get()->GetMessageManager()->DebugLog("Removing ancestor string from %s", token->m_Name.c_str(), token->m_Name.c_str());
			token->m_AncestorsString.Clear();
		}
	}
}

bool Parser::ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks)
{
	ParserThreadOptions opts;
	opts.wantPreprocessor = m_Options.wantPreprocessor;
	opts.useBuffer = true;
	opts.bufferSkipBlocks = bufferSkipBlocks;
	return Parse(buffer, isLocal, opts);
}

void Parser::BatchParse(const wxArrayString& filenames)
{
    m_Pool.BatchBegin();
    for (unsigned int i = 0; i < filenames.GetCount(); ++i)
        Parse(filenames[i]);
    m_Pool.BatchEnd();
}

bool Parser::Parse(const wxString& filename, bool isLocal)
{
	ParserThreadOptions opts;
	opts.wantPreprocessor = m_Options.wantPreprocessor;
	opts.useBuffer = false;
	opts.bufferSkipBlocks = false;
	return Parse(UnixFilename(filename), isLocal, opts);
}

bool Parser::Parse(const wxString& bufferOrFilename, bool isLocal, ParserThreadOptions& opts)
{
	wxString buffOrFile = bufferOrFilename;

	wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	bool parsed = !opts.useBuffer && m_ParsedFiles.Index(buffOrFile) != wxNOT_FOUND;
	delete lock;
	if (parsed)
	{
#ifndef STANDALONE
//        Manager::Get()->GetMessageManager()->DebugLog("%s is already parsed", buffOrFile.c_str());
#endif
		return false; // already parsed
    }

	ParserThread* thread = new ParserThread(this,&this->m_abort_flag,
											buffOrFile,
											isLocal,
											opts,
											&m_Tokens);
	if (!opts.useBuffer)
	{
//		lock = new wxMutexLocker(s_mutexListProtection);
		m_ParsedFiles.Add(buffOrFile);
//	    LOGSTREAM << "Adding task for: " << buffOrFile << '\n';
		m_Pool.AddTask(thread, true);
//		delete lock;
		return true;
	}
	else
	{
		bool ret = thread->Parse();
		LinkInheritance(true);
		delete thread;
		return ret;
	}
}

bool Parser::ParseBufferForFunctions(const wxString& buffer)
{
	ParserThreadOptions opts;
	opts.wantPreprocessor = m_Options.wantPreprocessor;
	opts.useBuffer = false;
	opts.bufferSkipBlocks = false;
	ParserThread* thread = new ParserThread(this,&this->m_abort_flag,
											wxEmptyString,
											false,
											opts,
											&m_Tokens);
	return thread->ParseBufferForFunctions(buffer);
}

bool Parser::RemoveFile(const wxString& filename)
{
	wxMutexLocker lock(s_mutexListProtection);
	wxMutexLocker lock1(s_mutexProtection);

	wxString file = UnixFilename(filename);
	if (m_ParsedFiles.Index(file) != wxNOT_FOUND)
	{
		// only if it has been parsed before...
		// delete any entries that belong to the file in question
		// FIXME: what happens with entries *linked* to this entry?
		unsigned int i = 0;
		while (i < m_Tokens.GetCount())
		{
			if (m_Tokens[i]->m_Filename.Matches(file))
				m_Tokens.RemoveAt(i);
			else
				++i;
		}
		m_ParsedFiles.Remove(file);
	}
	else
		return false;
	return true;
}

bool Parser::Reparse(const wxString& filename, bool isLocal)
{
	if (!Done())
		return false; // if still parsing, exit with error

	wxString file = UnixFilename(filename);
	Manager::Get()->GetMessageManager()->DebugLog(_("Reparsing %s"), file.c_str());
	RemoveFile(file);
	ClearTemporaries();
	wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	m_ReparsedFiles.Add(file);
	delete lock;

	return Parse(file, isLocal);
}

void Parser::Clear()
{
    DisconnectEvents();
	TerminateAllThreads();
	wxSafeYield();
	wxSleep(0);

	m_ParsedFiles.Clear();
	m_ReparsedFiles.Clear();
	m_IncludeDirs.Clear();

	wxMutexLocker lock(s_mutexProtection);
	WX_CLEAR_ARRAY(m_Tokens);
	m_Tokens.Clear();

	wxSafeYield();
	ConnectEvents();

	m_UsingCache = false;
	m_CacheFilesCount = 0;
	m_CacheTokensCount = 0;
}

void Parser::ClearTemporaries()
{
	if (!Done())
		return;

	unsigned int i = 0;
	while (i < m_Tokens.GetCount())
	{
		Token* token = m_Tokens[i];
		if (token->m_IsTemporary)
			m_Tokens.RemoveAt(i);
		else
			++i;
	}
}

void Parser::TerminateAllThreads()
{
    wxMutexLocker lock(s_mutexListProtection);
    m_Pool.AbortAllTasks();
}

void Parser::PauseAllThreads()
{
//    wxLogNull ln; // no other logging
//	wxMutexLocker lock(s_mutexListProtection);
//	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
//		m_Threads[i]->Pause();
}

void Parser::ResumeAllThreads()
{
//    wxLogNull ln; // no other logging
//	wxMutexLocker lock(s_mutexListProtection);
//	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
//		m_Threads[i]->Resume();
}

void Parser::OnStartThread(CodeBlocksEvent& event)
{
    event.Skip();
}

void Parser::OnEndThread(CodeBlocksEvent& event)
{
    event.Skip();
}

void Parser::OnAllThreadsDone(CodeBlocksEvent& event)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, PARSER_END);
    evt.SetInt((int)this);
	wxPostEvent(m_pParent, evt);
}

void Parser::OnNewToken(wxCommandEvent& event)
{
#ifndef STANDALONE
	Manager::Get()->GetMessageManager()->DebugLog(event.GetString());
#endif
}

void Parser::OnParseFile(wxCommandEvent& event)
{
//    LOGSTREAM << "Parser::OnParseFile: " << event.GetString() << "\n";
	// a ParserThread ran into an #include directive
	// it's up to us to decide to parse this file...

	if ((event.GetInt() == 0 && !m_Options.followLocalIncludes) ||
		(event.GetInt() == 1 && !m_Options.followGlobalIncludes))
		return;

	// the string is thread's_filename+included_filename
	wxString filename = event.GetString();
	int idx = filename.First('+');
	wxFileName fname;
	wxFileName source;
	wxString base;

	if (idx == -1)
		return;
	fname.Assign(filename.Mid(idx + 1));
	source.Assign(filename.Left(idx - 1));

	if (event.GetInt() == 0)
		base = source.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	else
	{
		// loop through all known include dirs
		// and locate the file...
		for (unsigned int i = 0; i < m_IncludeDirs.GetCount(); ++i)
		{
			base = m_IncludeDirs[i];
			wxFileName tmp = fname;
			tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
			if (wxFileExists(tmp.GetFullPath()))
				break;
		}
	}

	fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
	filename = fname.GetFullPath();

	/*wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	bool abort = m_ReparsedFiles.Index(filename) != wxNOT_FOUND;
	delete lock;
	if (abort)
		return; // the file is being re-parsed; don't follow includes
	*/

	if (m_ParsedFiles.Index(filename) != wxNOT_FOUND) // parsed file
        return;
//	Manager::Get()->GetMessageManager()->DebugLog("Adding in parse queue: %s", filename.c_str());

	bool res = false;
	if (m_ReparsedFiles.Index(source.GetFullPath()) != wxNOT_FOUND) // reparsing file
		res = Reparse(filename, event.GetInt() == 0);
	else
		res = Parse(filename, event.GetInt() == 0);
	if (res)
	{
#if 0
#ifndef STANDALONE
	Manager::Get()->GetMessageManager()->DebugLog("Adding in parse queue: %s", filename.c_str());
#endif
#endif
	}
}

void Parser::BuildTree(wxTreeCtrl& tree)
{
	if (!Done())
		return;

	tree.Freeze();
    tree.DeleteAllItems();

#ifndef STANDALONE
	tree.SetImageList(m_pImageList);
#endif
	m_RootNode = tree.AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER);
	if (m_BrowserOptions.viewFlat)
	{
		for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
		{
			Token* token = m_Tokens[x];
			if (!token->m_pParent && // base (no parent)
				token->m_IsLocal) // local symbols only
				AddTreeNode(tree, m_RootNode, token);
		}
		tree.Expand(m_RootNode);
		tree.Thaw();
		return;
	}

	wxTreeItemId globalNS = tree.AppendItem(m_RootNode, _("Global namespace"), PARSER_IMG_NAMESPACE);
    AddTreeNamespace(tree, globalNS, 0L);
    BuildTreeNamespace(tree, m_RootNode, 0L);
	tree.Expand(m_RootNode);
	tree.Thaw();
}

void Parser::BuildTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent)
{
	for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
	{
		Token* token = m_Tokens[x];
		if (token->m_pParent == parent &&
			token->m_IsLocal && // local symbols only
			token->m_TokenKind == tkNamespace) // namespaces
        {
            ClassTreeData* ctd = new ClassTreeData(token);
            wxTreeItemId newNS = tree.AppendItem(parentNode, token->m_Name, PARSER_IMG_NAMESPACE, -1, ctd);
            BuildTreeNamespace(tree, newNS, token);
            AddTreeNamespace(tree, newNS, token);
        }
	}
}

void Parser::AddTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent)
{
	wxTreeItemId node = tree.AppendItem(parentNode, _("Classes"), PARSER_IMG_CLASS_FOLDER);
	for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
	{
		Token* token = m_Tokens[x];
		if (token->m_pParent == parent && // parent matches
			token->m_IsLocal && // local symbols only
			token->m_TokenKind == tkClass) // classes
        {
			AddTreeNode(tree, node, token);
        }
	}
	node = tree.AppendItem(parentNode, _("Enums"), PARSER_IMG_ENUMS_FOLDER);
	for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
	{
		Token* token = m_Tokens[x];
		if (token->m_pParent == parent && // parent matches
			token->m_IsLocal && // local symbols only
			token->m_TokenKind == tkEnum) // enums
        {
			AddTreeNode(tree, node, token);
        }
	}
	node = tree.AppendItem(parentNode, _("Preprocessor"), PARSER_IMG_PREPROC_FOLDER);
	for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
	{
		Token* token = m_Tokens[x];
		if (token->m_pParent == parent && // parent matches
			token->m_IsLocal && // local symbols only
			token->m_TokenKind == tkPreprocessor) // preprocessor
        {
			AddTreeNode(tree, node, token);
        }
	}
	node = tree.AppendItem(parentNode, _("Others"), PARSER_IMG_OTHERS_FOLDER);
	for (unsigned int x = 0; x < m_Tokens.GetCount(); ++x)
	{
		Token* token = m_Tokens[x];
		if (token->m_pParent == parent && // parent matches
			token->m_IsLocal && // local symbols only
			(token->m_TokenKind == tkEnumerator || // enumerators
			token->m_TokenKind == tkFunction || // functions
			token->m_TokenKind == tkVariable || // variables
			token->m_TokenKind == tkUndefined)) // others
        {
			AddTreeNode(tree, node, token);
        }
	}
}

void Parser::AddTreeNode(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* token, bool childrenOnly)
{
    if (!token)
        return;
	ClassTreeData* ctd = new ClassTreeData(token);
	int image = -1;
#ifndef STANDALONE
	image = GetTokenKindImage(token);
#endif
	wxString str = token->m_Name + token->m_Args;
	if (!token->m_ActualType.IsEmpty())
		 str = str + _T(" : ") + token->m_ActualType;
	wxTreeItemId node = childrenOnly ? parentNode : tree.AppendItem(parentNode, str, image, -1, ctd);

	// add children
	for (unsigned int i = 0; i < token->m_Children.GetCount(); ++i)
	{
		Token* childToken = token->m_Children[i];
		AddTreeNode(tree, node, childToken);
	}

	if (!m_BrowserOptions.showInheritance || (token->m_TokenKind != tkClass && token->m_TokenKind != tkNamespace))
		return;
	// add ancestor's children
	for (unsigned int x = 0; x < token->m_Ancestors.GetCount(); ++x)
	{
		Token* ancestor = token->m_Ancestors[x];
		AddTreeNode(tree, node, ancestor, true);
	}
}
