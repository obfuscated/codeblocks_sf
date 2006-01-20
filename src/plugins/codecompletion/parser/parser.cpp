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

#include <sdk.h>
#include <wx/log.h>
#include <wx/app.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/progdlg.h>
#include "parser.h"
#include "../classbrowser.h"
#ifndef STANDALONE
	#include <configmanager.h>
	#include <messagemanager.h>
	#include <editormanager.h>
	#include <manager.h>
	#include <globals.h>
#endif // STANDALONE

static const char CACHE_MAGIC[] = "CCCACHE_1_2";

static wxCriticalSection s_mutexListProtection;
int PARSER_END = wxNewId();
static int idPool = wxNewId();
int TIMER_ID = wxNewId();
int BATCH_TIMER_ID = wxNewId();

BEGIN_EVENT_TABLE(Parser, wxEvtHandler)
//	EVT_MENU(NEW_TOKEN, Parser::OnNewToken)
//	EVT_MENU(FILE_NEEDS_PARSING, Parser::OnParseFile)
	EVT_THREADTASK_STARTED(idPool, Parser::OnStartThread)
	EVT_THREADTASK_ENDED(idPool, Parser::OnEndThread)
    EVT_THREADTASK_ALLDONE(idPool, Parser::OnAllThreadsDone)
    EVT_TIMER(TIMER_ID, Parser::OnTimer)
    EVT_TIMER(BATCH_TIMER_ID, Parser::OnBatchTimer)
END_EVENT_TABLE()

Parser::Parser(wxEvtHandler* parent)
	: m_MaxThreadsCount(8),
	m_pParent(parent)
#ifndef STANDALONE
	,m_pImageList(0L)
#endif
    ,m_abort_flag(false),
    m_UsingCache(false),
    m_Pool(this, idPool),
    m_pTokens(0),
    m_pTempTokens(0),
    m_NeedsReparse(false),
    m_IsBatch(false),
    m_pClassBrowser(0),
    m_TreeBuildingStatus(0),
    m_TreeBuildingTokenIdx(0),
    m_timer(this, TIMER_ID),
    m_batchtimer(this,BATCH_TIMER_ID)
{
    m_pTokens = new TokensTree;
    m_pTempTokens = new TokensTree;
    m_LocalFiles.clear();
	ReadOptions();
#ifndef STANDALONE
	m_pImageList = new wxImageList(16, 16);
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::GetDataFolder() + _T("/images/codecompletion/");
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
    if(m_pClassBrowser && m_pClassBrowser->GetParserPtr() == this)
        m_pClassBrowser->UnlinkParser();
    m_TreeBuildingStatus = 0;
    m_pClassBrowser = NULL;

    DisconnectEvents();
	Clear();
#ifndef STANDALONE
	delete m_pImageList;
    delete m_pTempTokens;
	delete m_pTokens;
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
	m_BrowserOptions.showAllSymbols = false;
#else // !STANDALONE
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
	m_MaxThreadsCount = cfg->ReadInt(_T("/max_threads"), wxThread::GetCPUCount());
	m_Options.followLocalIncludes = cfg->ReadBool(_T("/parser_follow_local_includes"), false);
	m_Options.followGlobalIncludes = cfg->ReadBool(_T("/parser_follow_global_includes"), false);
	m_Options.caseSensitive = cfg->ReadBool(_T("/case_sensitive"), false);
	m_Options.useSmartSense = cfg->ReadBool(_T("/use_SmartSense"), true);
	m_Options.wantPreprocessor = cfg->ReadBool(_T("/want_preprocessor"), false);
	m_BrowserOptions.showInheritance = cfg->ReadBool(_T("/browser_show_inheritance"), false);
	m_BrowserOptions.viewFlat = cfg->ReadBool(_T("/browser_view_flat"), false);
	m_BrowserOptions.showAllSymbols = cfg->ReadBool(_T("/show_all_symbols"), false);
#endif // STANDALONE
}

void Parser::WriteOptions()
{
#ifndef STANDALONE
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
	cfg->Write(_T("/max_threads"), (int)m_MaxThreadsCount);
	cfg->Write(_T("/parser_follow_local_includes"), m_Options.followLocalIncludes);
	cfg->Write(_T("/parser_follow_global_includes"), m_Options.followGlobalIncludes);
	cfg->Write(_T("/case_sensitive"), m_Options.caseSensitive);
	cfg->Write(_T("/use_SmartSense"), m_Options.useSmartSense);
	cfg->Write(_T("/want_preprocessor"), m_Options.wantPreprocessor);
	cfg->Write(_T("/browser_show_inheritance"), m_BrowserOptions.showInheritance);
	cfg->Write(_T("/show_all_symbols"), m_BrowserOptions.showAllSymbols);
	cfg->Write(_T("/browser_view_flat"), m_BrowserOptions.viewFlat);
#endif // STANDALONE
}

bool Parser::CacheNeedsUpdate()
{
    if (m_UsingCache)
    {
        wxCriticalSectionLocker lock(s_mutexProtection);
        return m_pTokens->m_modified;
    }
    return true;
}

unsigned int Parser::GetFilesCount()
{
	wxCriticalSectionLocker lock(s_mutexProtection);
	return m_pTokens->m_FilesMap.size();
}

bool Parser::Done()
{
    wxCriticalSectionLocker lock(s_mutexListProtection);
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

Token* Parser::FindTokenByName(const wxString& name, bool globalsOnly, short int kindMask) const
{
    wxCriticalSectionLocker lock(s_mutexProtection);
    int result = m_pTokens->TokenExists(name,-1,kindMask);
    return m_pTokens->at(result);
}

Token* Parser::FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance, short int kindMask) const
{
	if (!parent)
		return FindTokenByName(name, false, kindMask);
    Token* result = 0;
    wxCriticalSectionLocker *lock = 0;
    {
        lock = new wxCriticalSectionLocker(s_mutexProtection);
        result = m_pTokens->at(m_pTokens->TokenExists(name,parent->GetSelf(),kindMask));
        delete lock;
    }
    if(!result && useInheritance)
    {
        lock = new wxCriticalSectionLocker(s_mutexProtection);
        TokenIdxSet::iterator it;
        for(it = parent->m_Ancestors.begin();it != parent->m_Ancestors.end();++it)
        {
            Token* ancestor = m_pTokens->at(*it);
            result = FindChildTokenByName(ancestor, name, true, kindMask);
            if(result)
                break;
        }
        delete lock;
    }
    return result;
}

Token* Parser::FindTokenByDisplayName(const wxString& name) const
{
	wxCriticalSectionLocker lock(s_mutexProtection);
	int idx = m_pTokens->FindTokenByDisplayName(name);
	return m_pTokens->at(idx);
}

size_t Parser::FindMatches(const wxString& s,TokenList& result,bool caseSensitive,bool is_prefix,bool markedonly)
{
    result.clear();
    TokenIdxSet tmpresult;
    wxCriticalSectionLocker lock(s_mutexProtection);
    if(!m_pTokens->FindMatches(s,tmpresult,caseSensitive,is_prefix))
        return 0;

    TokenIdxSet::iterator it;
    for(it = tmpresult.begin();it!=tmpresult.end();++it)
    {
        Token* token = m_pTokens->at(*it);
        if(token && (!markedonly || token->m_Bool))
        result.push_back(token);
    }
    return result.size();
}

void Parser::LinkInheritance(bool tempsOnly)
{
	wxCriticalSectionLocker lock(s_mutexProtection);
	(tempsOnly ? m_pTempTokens :  m_pTokens)->RecalcData();
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
    for (unsigned int i = 0; i < filenames.GetCount(); ++i)
        Parse(filenames[i]);
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
	bool parsed = false;

	if(!opts.useBuffer){
        wxCriticalSectionLocker lock(s_mutexProtection);
        size_t index = m_pTokens->GetFileIndex(buffOrFile);
        parsed = (m_pTokens->m_FilesMap.count(index) &&
           m_pTokens->m_FilesStatus[index]!=fpsNotParsed &&
           !m_pTokens->m_FilesToBeReparsed.count(index)
           );
	}
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
											(opts.useBuffer ? m_pTempTokens : m_pTokens));
    if (!opts.useBuffer)
	{
//		lock = new wxCriticalSectionLocker(s_mutexListProtection);
//	    LOGSTREAM << "Adding task for: " << buffOrFile << '\n';
#ifdef CODECOMPLETION_PROFILING
        thread->Parse();
#else
		if(!m_IsBatch)
		{
            m_IsBatch = true;
            m_Pool.BatchBegin();
		}
		m_Pool.AddTask(thread, true);
        m_batchtimer.Start(100,wxTIMER_ONE_SHOT);
        // For every parse, the timer is reset to -100 ms, so there is a
        // 100 ms. tolerance for the next parse to get queued. After that,
        // the timer will trigger the event that will start the batch job.

#endif
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
											m_pTempTokens);
	return thread->ParseBufferForFunctions(buffer);
}

bool Parser::RemoveFile(const wxString& filename)
{
	if(!Done())
        return false; // Can't alter the tokens tree if parsing has not finished
	wxString file = UnixFilename(filename);
	wxCriticalSectionLocker lock(s_mutexProtection);
    size_t index = m_pTokens->GetFileIndex(file);
	bool result = m_pTokens->m_FilesStatus.count(index);

    m_pTokens->RemoveFile(filename);
    m_pTokens->m_FilesMap.erase(index);
    m_pTokens->m_FilesStatus.erase(index);
    m_pTokens->m_FilesToBeReparsed.erase(index);
    m_pTokens->m_modified = true;
	return result;
}

bool Parser::Reparse(const wxString& filename, bool isLocal)
{
	if (!Done())
		return false; // if still parsing, exit with error
	wxString file = UnixFilename(filename);
	if(isLocal)
        m_LocalFiles.insert(filename);
    else
        m_LocalFiles.erase(filename);
	{
	    wxCriticalSectionLocker lock(s_mutexProtection);
	    m_pTokens->FlagFileForReparsing(file);
	}
    m_NeedsReparse = true;
    m_timer.Start(200,wxTIMER_ONE_SHOT);
    return true;
}

void Parser::Clear()
{
//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Disconnecting events..."));
    DisconnectEvents();
//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Terminating all threads..."));
	TerminateAllThreads();
    Manager::ProcessPendingEvents();
	wxSleep(0);

	{
	wxCriticalSectionLocker lockl(s_mutexListProtection);
//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Clearing 'm_IncludeDirs'..."));
	m_IncludeDirs.Clear();
	}

//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Locking s_mutexProtection and clearing m_pTokens->.."));
	wxCriticalSectionLocker lock(s_mutexProtection);
    m_pTokens->clear();
    m_pTempTokens->clear();
    m_LocalFiles.clear();

//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: wxSafeYield..."));
	Manager::ProcessPendingEvents();
//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Connecting Events..."));
	ConnectEvents();

	m_UsingCache = false;
	m_abort_flag = false;
//    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::Clear: Done."));
}

bool Parser::ReadFromCache(wxInputStream* f)
{
    bool result = false;
    wxCriticalSectionLocker lock(s_mutexProtection);

    char CACHE_MAGIC_READ[] = "           ";
    m_pTokens->clear(); // Clear data

    // File format is like this:
    //
    // CACHE_MAGIC
    // Number of parsed files
    // Number of tokens
    // Parsed files
    // Tokens
    // EOF

//  Begin loading process
    do
    {

        // keep a backup of include dirs
        if (f->Read(CACHE_MAGIC_READ, sizeof(CACHE_MAGIC_READ)).LastRead() != sizeof(CACHE_MAGIC_READ) ||
            strncmp(CACHE_MAGIC, CACHE_MAGIC_READ, sizeof(CACHE_MAGIC_READ) != 0))
            break;
        int fcount = 0, actual_fcount = 0;
        int tcount = 0, actual_tcount = 0;
        int idx;
        if (!LoadIntFromFile(f, &fcount))
            break;
        if (!LoadIntFromFile(f, &tcount))
            break;
        if (fcount < 0)
            break;
        if (tcount < 0)
            break;

        wxString file;
        int nonempty_token = 0;
        Token* token = 0;
        do // do while-false block
        {
            // Filenames
            int i;
            for (i = 0; i < fcount && !f->Eof(); ++i)
            {
                if(!LoadIntFromFile(f,&idx)) // Filename index
                    break;
                if(idx != i)
                    break;
                if(!LoadStringFromFile(f,file)) // Filename data
                    break;
                if(!idx)
                    file.Clear();
                if(file.IsEmpty())
                    idx = 0;
                m_pTokens->m_FilenamesMap.insert(file);
                actual_fcount++;
            }
            result = (actual_fcount == fcount);
            if(!result)
                break;
            if(tcount)
                m_pTokens->m_Tokens.resize(tcount,0);
            // Tokens
            for (i = 0; i < tcount && !f->Eof(); ++i)
            {
                token = 0;
                if (!LoadIntFromFile(f, &nonempty_token))
                break;
                if(nonempty_token != 0)
                {
                    token = new Token();
                    if (!token->SerializeIn(f))
                    {
                        delete token;
                        token = 0;
                        break;
                    }
                    m_pTokens->insert(i,token);
                }
                ++actual_tcount;
            }
            if(actual_tcount != tcount)
                break;
            m_pTokens->RecalcFreeList();
            result = true;
        }while(false);

    }
    while(false);

//  End loading process

    if(result)
        m_UsingCache = true;
    else
        m_pTokens->clear();
    m_pTokens->m_modified = false;
    return result;
}

bool Parser::WriteToCache(wxOutputStream* f)
{
    bool result = false;
    wxCriticalSectionLocker lock(s_mutexProtection);
//  Begin saving process

    size_t tcount = m_pTokens->m_Tokens.size();
    size_t fcount = m_pTokens->m_FilenamesMap.size();
    size_t i = 0;

    // write cache magic
    f->Write(CACHE_MAGIC, sizeof(CACHE_MAGIC));

    SaveIntToFile(f, fcount); // num parsed files
    SaveIntToFile(f, tcount); // num tokens

    // Filenames
    for(i = 0; i < fcount; ++i)
    {
        SaveIntToFile(f,i);
        SaveStringToFile(f,m_pTokens->m_FilenamesMap.GetString(i));
    }

    // Tokens

    for (i = 0; i < tcount; ++i)
    {
        // Manager::Get()->GetMessageManager()->DebugLog(_("Token #%d, offset %d"),i,f->TellO());
        Token* token = m_pTokens->at(i);
        SaveIntToFile(f,(token!=0) ? 1 : 0);
        if(token)
            token->SerializeOut(f);
    }

    result = true;

    if(result)
        m_pTokens->m_modified = false;
    return result;
//  End saving process
    return result;
}

void Parser::TerminateAllThreads()
{
    Manager::Get()->GetMessageManager()->DebugLog(_("Parser::TerminateAllThreads: Aborting all tasks..."));
    m_Pool.AbortAllTasks();
}

void Parser::PauseAllThreads()
{
//    wxLogNull ln; // no other logging
//	wxCriticalSectionLocker lock(s_mutexListProtection);
//	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
//		m_Threads[i]->Pause();
}

void Parser::ResumeAllThreads()
{
//    wxLogNull ln; // no other logging
//	wxCriticalSectionLocker lock(s_mutexListProtection);
//	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
//		m_Threads[i]->Resume();
}

void Parser::AddIncludeDir(const wxString& file)
{
	if(m_IncludeDirs.Index(file) == wxNOT_FOUND)
	{
//    	Manager::Get()->GetMessageManager()->DebugLog(_("Adding %s"), file.c_str());
		m_IncludeDirs.Add(file);
	}
} // end of AddIncludeDir

wxArrayString Parser::FindFileInIncludeDirs(const wxString& file)
{
	wxArrayString FoundSet;
	for(size_t idxSearch = 0; idxSearch < m_IncludeDirs.GetCount(); ++idxSearch)
	{
		wxString base = m_IncludeDirs[idxSearch];
		wxFileName tmp = file;
		tmp.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
		if(wxFileExists(tmp.GetFullPath()))
		{
			FoundSet.Add(tmp.GetFullPath());
		}
	} // end for : idx : idxSearch
//	Manager::Get()->GetMessageManager()->DebugLog(_("Searching %s"), file.c_str());
//	Manager::Get()->GetMessageManager()->DebugLog(_("Found %d"), FoundSet.GetCount());
	return FoundSet;
} // end of FindFileInIncludeDirs

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
    LinkInheritance(false);
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, PARSER_END);
    evt.SetClientData(this);
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
	wxString tgt = filename.AfterFirst(_T('+'));
	if (tgt.IsEmpty())
        return;
	wxString src = filename.BeforeFirst(_T('+'));
	fname.Assign(tgt);
	source.Assign(src);

    // search
	if (m_Options.followGlobalIncludes)
	{
		wxArrayString FoundSet = FindFileInIncludeDirs(tgt);
		wxString FirstFound = FoundSet.GetCount()?FoundSet[0]:_T("");
		wxString g = UnixFilename(FirstFound);
	    if (g.IsEmpty())
	    {
//            Manager::Get()->GetMessageManager()->DebugLog(_T("? No include looking for %s, ? %s"), tgt.c_str(), filename.c_str());
            return;
	    }
	    filename = g;
	}

    Parse(filename, event.GetInt() == 0);
}

void Parser::BuildTree(wxTreeCtrl& tree)
{
	if (!Done())
		return;

	tree.Freeze();
    tree.DeleteAllItems();
    TokenFilesSet currset;
    currset.clear();
    Token* token = 0;
#ifndef STANDALONE
	tree.SetImageList(m_pImageList);

    wxString fname(_T(""));
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        fname = ed->GetFilename().BeforeLast(_T('.'));

    // "mark" tokens based on scope
    bool fnameEmpty = fname.IsEmpty();
    fname.Append(_T('.'));
    if(!fnameEmpty && !m_BrowserOptions.showAllSymbols)
    {
        for(size_t i = 1; i < m_pTokens->m_FilenamesMap.size(); ++i)
        {
            if(m_pTokens->m_FilenamesMap.GetString(i).StartsWith(fname))
                currset.insert(i);
        }
    }

#endif

	m_RootNode = tree.AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER);
	if (m_BrowserOptions.viewFlat)
	{
        TokenIdxSet::iterator it,it_end;
        it = m_pTokens->m_GlobalNameSpace.begin();
        it_end = m_pTokens->m_GlobalNameSpace.end();

        for(;it != it_end;++it)
        {
            token = m_pTokens->at(*it);
            if(!token || !token->m_IsLocal || token->m_ParentIndex!=-1 || !token->MatchesFiles(currset))
                continue;
            AddTreeNode(tree, m_RootNode, token);
        }
		tree.SortChildren(m_RootNode);
	}
	else
	{
        wxTreeItemId globalNS = tree.AppendItem(m_RootNode, _("Global namespace"), PARSER_IMG_NAMESPACE);
        AddTreeNamespace(tree, globalNS, 0,currset);
        BuildTreeNamespace(tree, m_RootNode, 0,currset);
	}

	tree.Expand(m_RootNode);
	tree.Thaw();
	// wxString memdump = m_pTokens->m_Tree.Serialize();
	// Manager::Get()->GetMessageManager()->DebugLog(memdump);
}

void Parser::BuildTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent, const TokenFilesSet& currset)
{
	TokenIdxSet::iterator it,it_end;
	int parentidx;
	if(!parent)
	{
        it = m_pTokens->m_TopNameSpaces.begin();
        it_end = m_pTokens->m_TopNameSpaces.end();
        parentidx = -1;
	}
    else
    {
        it = parent->m_Children.begin();
        it_end = parent->m_Children.end();
        parentidx = parent->GetSelf();
    }

	for(;it != it_end; it++)
	{
	    Token* token = m_pTokens->at(*it);
	    if(!token || /* !token->m_Bool || */ !token->m_IsLocal || token->m_TokenKind != tkNamespace)
            continue;
        if(currset.size() && !token->MatchesFiles(currset))
            continue;
        ClassTreeData* ctd = new ClassTreeData(token);
        wxTreeItemId newNS = tree.AppendItem(parentNode, token->m_Name, PARSER_IMG_NAMESPACE, -1, ctd);
        BuildTreeNamespace(tree, newNS, token, currset);
        AddTreeNamespace(tree, newNS, token, currset);
	}
    tree.SortChildren(parentNode);
}

void Parser::AddTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent,const TokenFilesSet& currset)
{
	TokenIdxSet::iterator it,it_end;
	int parentidx;
	if(!parent)
	{
        it = m_pTokens->m_GlobalNameSpace.begin();
        it_end = m_pTokens->m_GlobalNameSpace.end();
        parentidx = -1;
	}
    else
    {
        it = parent->m_Children.begin();
        it_end = parent->m_Children.end();
        parentidx = parent->GetSelf();
    }

	bool has_classes = false,has_enums = false,has_preprocessor = false,has_others = false;
	wxTreeItemId node_classes;
	wxTreeItemId node_enums;
	wxTreeItemId node_preprocessor;
	wxTreeItemId node_others;
	wxTreeItemId* curnode = 0;

	for(;it != it_end; it++)
	{
	    Token* token = m_pTokens->at(*it);
	    if(!token || /* !token->m_Bool || */ !token->m_IsLocal)
            continue;
        if(currset.size() && !token->MatchesFiles(currset))
            continue;

        switch(token->m_TokenKind)
        {
            case tkClass:
                    if(!has_classes)
                    {
                        has_classes = true;
                        node_classes = tree.AppendItem(parentNode, _("Classes"), PARSER_IMG_CLASS_FOLDER);
                    }
                    curnode = &node_classes;
                    break;
            case tkEnum:
                    if(!has_enums)
                    {
                        has_enums = true;
                        node_enums = tree.AppendItem(parentNode, _("Enums"), PARSER_IMG_ENUMS_FOLDER);
                    }
                    curnode = &node_enums;
                    break;
            case tkPreprocessor:
                    if(!has_preprocessor)
                    {
                        has_preprocessor = true;
                        node_preprocessor = tree.AppendItem(parentNode, _("Preprocessor"), PARSER_IMG_PREPROC_FOLDER);
                    }
                    curnode = &node_preprocessor;
                    break;
            case tkEnumerator:
            case tkFunction:
            case tkVariable:
            case tkUndefined:
                    if(!has_others)
                    {
                        has_others = true;
                        node_others = tree.AppendItem(parentNode, _("Others"), PARSER_IMG_OTHERS_FOLDER);
                    }
                    curnode = &node_others;
                    break;
            default:curnode = 0;
        }
        if(curnode)
            AddTreeNode(tree, *curnode, token);
	}
    if(has_classes)
        tree.SortChildren(node_classes);
    if(has_enums)
        tree.SortChildren(node_enums);
    if(has_preprocessor)
        tree.SortChildren(node_preprocessor);
    if(has_others)
        tree.SortChildren(node_others);
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
	TokenIdxSet::iterator it;
	for(it=token->m_Children.begin();it!=token->m_Children.end();++it)
	{
	    AddTreeNode(tree, node, m_pTokens->at(*it));
	}

	if (!m_BrowserOptions.showInheritance || (token->m_TokenKind != tkClass && token->m_TokenKind != tkNamespace))
		return;
	// add ancestor's children
	for(it=token->m_Ancestors.begin();it!=token->m_Ancestors.end();++it)
	{
	    AddTreeNode(tree, node, m_pTokens->at(*it),true);
	}

    tree.SortChildren(node);
}

void Parser::AbortBuildingTree()
{
    if(m_TreeBuildingStatus == 2)
    {
        if(m_pClassBrowser && m_pClassBrowser->GetParserPtr() == this && m_pClassBrowser->GetTree())
        {
            m_pClassBrowser->GetTree()->DeleteAllItems();
            m_pClassBrowser->GetTree()->Thaw();
        }
    }
    m_TreeBuildingStatus = 0;
}

void Parser::OnTimer(wxTimerEvent& event)
{
    ReparseModifiedFiles();
    event.Skip();
}

void Parser::OnBatchTimer(wxTimerEvent& event)
{
    if(m_IsBatch)
    {
        m_IsBatch = false;
        StartTimer(); // To add more to the confusion, we have a "timer"
        // stopwatch to measure the parsing time.
        m_Pool.BatchEnd();
    }
}

bool Parser::ReparseModifiedFiles()
{
    if(!m_NeedsReparse || !m_Pool.Done())
        return false;
    Manager::Get()->GetMessageManager()->DebugLog(_("Reparsing saved files..."));
    m_NeedsReparse = false;
    int numfiles = 0;
    vector<wxString> files_list;
    {
        wxCriticalSectionLocker lock(s_mutexProtection);
        TokenFilesSet::iterator it;
        for(it = m_pTokens->m_FilesToBeReparsed.begin(); it != m_pTokens->m_FilesToBeReparsed.end(); ++it)
        {
            m_pTokens->RemoveFile(*it);
            files_list.push_back(m_pTokens->m_FilenamesMap.GetString(*it));
            ++numfiles;
        }
    }
    if(!numfiles)
        return true;
    for(size_t i = 0; i < files_list.size();++i)
    {
        Parse(files_list[i],m_LocalFiles.count(files_list[i]));
    }
    return true;
}
