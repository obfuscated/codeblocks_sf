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
#include "parser.h"
#ifndef STANDALONE
	#include <configmanager.h>
	#include <messagemanager.h>
	#include <manager.h>
	#include <globals.h>
#endif // STANDALONE

static wxMutex s_mutexListProtection;
int PARSER_END = wxNewId();

BEGIN_EVENT_TABLE(Parser, wxEvtHandler)
//	EVT_MENU(THREAD_START, Parser::OnStartThread)
//	EVT_MENU(THREAD_END, Parser::OnEndThread)
//	EVT_MENU(NEW_TOKEN, Parser::OnNewToken)
//	EVT_MENU(FILE_NEEDS_PARSING, Parser::OnParseFile)
END_EVENT_TABLE()

Parser::Parser(wxEvtHandler* parent)
	: m_MaxThreadsCount(8),
	m_pParent(parent)
#ifndef STANDALONE
	,m_pImageList(0L)
#endif
    ,m_abort_flag(false)
{
	ReadOptions();
#ifndef STANDALONE
	m_pImageList = new wxImageList(16, 16);
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::Get()->Read("data_path") + "/images/codecompletion/";
    // bitmaps must be added by order of PARSER_IMG_* consts
    bmp.LoadFile(prefix + "class_folder.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CLASS_FOLDER
    bmp.LoadFile(prefix + "class.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CLASS
    bmp.LoadFile(prefix + "ctor_private.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PRIVATE
    bmp.LoadFile(prefix + "ctor_protected.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PROTECTED
    bmp.LoadFile(prefix + "ctor_public.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_CTOR_PUBLIC
    bmp.LoadFile(prefix + "dtor_private.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PRIVATE
    bmp.LoadFile(prefix + "dtor_protected.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PROTECTED
    bmp.LoadFile(prefix + "dtor_public.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_DTOR_PUBLIC
    bmp.LoadFile(prefix + "method_private.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp.LoadFile(prefix + "method_protected.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PRIVATE
    bmp.LoadFile(prefix + "method_public.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_FUNC_PUBLIC
    bmp.LoadFile(prefix + "var_private.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PRIVATE
    bmp.LoadFile(prefix + "var_protected.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PROTECTED
    bmp.LoadFile(prefix + "var_public.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_VAR_PUBLIC
    bmp.LoadFile(prefix + "preproc.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_PREPROCESSOR
    bmp.LoadFile(prefix + "enum.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUM
    bmp.LoadFile(prefix + "enumerator.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUMERATOR
    bmp.LoadFile(prefix + "namespace.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_NAMESPACE
    bmp.LoadFile(prefix + "symbols_folder.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_SYMBOLS_FOLDER
    bmp.LoadFile(prefix + "enums_folder.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_ENUMS_FOLDER
    bmp.LoadFile(prefix + "preproc_folder.png", wxBITMAP_TYPE_PNG);
    m_pImageList->Add(bmp); // PARSER_IMG_PREPROC_FOLDER
    bmp.LoadFile(prefix + "others_folder.png", wxBITMAP_TYPE_PNG);
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
    Connect(THREAD_START, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnStartThread);
    Connect(THREAD_END, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnEndThread);
    Connect(NEW_TOKEN, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnNewToken);
    Connect(FILE_NEEDS_PARSING, -1, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnParseFile);
}

void Parser::DisconnectEvents()
{
    Disconnect(THREAD_START, -1, wxEVT_COMMAND_MENU_SELECTED);//,
    Disconnect(THREAD_END, -1, wxEVT_COMMAND_MENU_SELECTED);//,
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
	m_MaxThreadsCount = ConfigManager::Get()->Read("/code_completion/max_threads", 8);
	m_Options.followLocalIncludes = ConfigManager::Get()->Read("/code_completion/parser_follow_local_includes", 1L);
	m_Options.followGlobalIncludes = ConfigManager::Get()->Read("/code_completion/parser_follow_global_includes", 0L);
	m_Options.caseSensitive = ConfigManager::Get()->Read("/code_completion/case_sensitive", 1);
	m_Options.useSmartSense = ConfigManager::Get()->Read("/code_completion/use_SmartSense", 1);
	m_Options.wantPreprocessor = ConfigManager::Get()->Read("/code_completion/want_preprocessor", 0L);
	m_BrowserOptions.showInheritance = ConfigManager::Get()->Read("/code_completion/browser_show_inheritance", 0L);
	m_BrowserOptions.viewFlat = ConfigManager::Get()->Read("/code_completion/browser_view_flat", 0L);
#endif // STANDALONE
}

void Parser::WriteOptions()
{
#ifndef STANDALONE
	ConfigManager::Get()->Write("/code_completion/max_threads", (int)m_MaxThreadsCount);
	ConfigManager::Get()->Write("/code_completion/parser_follow_local_includes", m_Options.followLocalIncludes);
	ConfigManager::Get()->Write("/code_completion/parser_follow_global_includes", m_Options.followGlobalIncludes);
	ConfigManager::Get()->Write("/code_completion/case_sensitive", m_Options.caseSensitive);
	ConfigManager::Get()->Write("/code_completion/use_SmartSense", m_Options.useSmartSense);
	ConfigManager::Get()->Write("/code_completion/want_preprocessor", m_Options.wantPreprocessor);
	ConfigManager::Get()->Write("/code_completion/browser_show_inheritance", m_BrowserOptions.showInheritance);
	ConfigManager::Get()->Write("/code_completion/browser_view_flat", m_BrowserOptions.viewFlat);
#endif // STANDALONE
}

unsigned int Parser::GetThreadsCount()
{
	wxMutexLocker lock(s_mutexListProtection);
	return m_Threads.GetCount();
}

unsigned int Parser::GetFilesCount()
{
	wxMutexLocker lock(s_mutexListProtection);
	return m_ParsedFiles.GetCount();
}

unsigned int Parser::GetLeftThreadsCount()
{
	wxMutexLocker lock(s_mutexListProtection);
	return m_ThreadsStore.GetCount();
}

bool Parser::Done()
{
	wxMutexLocker lock(s_mutexListProtection);
	return !m_ThreadsStore.GetCount() && !m_Threads.GetCount();
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

Token* Parser::FindTokenByName(const wxString& name, bool globalsOnly)
{
	for (unsigned int i = 0; i < m_Tokens.GetCount(); ++i)
	{
		Token* token = m_Tokens[i];
		if (globalsOnly && token->m_pParent)
			continue;
		if (token->m_Name.Matches(name))
			return token;
	}
	return 0L;
}

Token* Parser::FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance)
{
	if (!parent)
		return FindTokenByName(name, false);

	for (unsigned int i = 0; i < parent->m_Children.GetCount(); ++i)
	{
		Token* token = parent->m_Children[i];
		if (token->m_Name.Matches(name))
			return token;
	}
	// not found; check ancestors now...
	if (useInheritance)
	{
		for (unsigned int i = 0; i < parent->m_Ancestors.GetCount(); ++i)
		{
			Token* inherited = FindChildTokenByName(parent->m_Ancestors[i], name, true);
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

void Parser::ScheduleThreads()
{
/*#ifndef STANDALONE
	Manager::Get()->GetMessageManager()->DebugLog("Parser: Scheduling threads");
	Manager::Get()->GetMessageManager()->DebugLog("Parser: Running thread count: %d", m_Threads.GetCount());
	Manager::Get()->GetMessageManager()->DebugLog("Parser: Max running thread count: %d", m_MaxThreadsCount);
	Manager::Get()->GetMessageManager()->DebugLog("Parser: Threads-in-store count: %d", m_ThreadsStore.GetCount());
#endif*/
	wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	if (m_Threads.GetCount() < m_MaxThreadsCount && m_ThreadsStore.GetCount())
	{
		ParserThread* thread = m_ThreadsStore[0];
		m_ThreadsStore.RemoveAt(0);
		m_Threads.Add(thread);
		thread->Run();
	}
	delete lock;
	
	if (Done())
	{
        lock = new wxMutexLocker(s_mutexListProtection);
		LinkInheritance();
		SortAllTokens();
		delete lock;
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, PARSER_END);
		event.SetInt((int)this);
		wxPostEvent(m_pParent, event);
	}
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
		if (parent1 == parent2)
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
		wxStringTokenizer tkz(token->m_AncestorsString, ",");
		while (tkz.HasMoreTokens())
		{
			wxString ancestor = tkz.GetNextToken();
			if (ancestor.IsEmpty())
				continue;
			//Manager::Get()->GetMessageManager()->DebugLog("Ancestor %s", ancestor.c_str());
			Token* ancestorToken = FindTokenByName(ancestor);
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
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
#ifndef STANDALONE
        Manager::Get()->GetMessageManager()->DebugLog("Can't create new thread!");
#endif
        thread->Delete();
		return false;
	}
	
	if (!opts.useBuffer)
	{
		lock = new wxMutexLocker(s_mutexListProtection);
		m_ThreadsStore.Add(thread);
		m_ParsedFiles.Add(buffOrFile);
		delete lock;
	
		ScheduleThreads();
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
	Manager::Get()->GetMessageManager()->DebugLog("Reparsing %s", file.c_str());
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
	ParserThread::abort(&this->m_abort_flag,false); // Quickly abort all threads
	while (m_Threads.GetCount())
	{
        ParserThread* pt = m_Threads.Item(0);
        if (pt)
        {
            pt->SetTokens(0);
            delete pt;
        }
        m_Threads.RemoveAt(0);
	}

	while (m_ThreadsStore.GetCount())
	{
        ParserThread* pt = m_ThreadsStore.Item(0);
        if (pt)
            pt->SetTokens(0);
        m_ThreadsStore.RemoveAt(0);
	}
}

void Parser::PauseAllThreads()
{
	wxMutexLocker lock(s_mutexListProtection);
	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
		m_Threads[i]->Pause();
}

void Parser::ResumeAllThreads()
{
	wxMutexLocker lock(s_mutexListProtection);
	for (unsigned int i = 0; i < m_Threads.GetCount(); ++i)
		m_Threads[i]->Resume();
}

wxString Parser::ThreadFilename(unsigned int idx)
{
	wxMutexLocker lock(s_mutexListProtection);
	return m_Threads[idx]->GetFilename();
}

void Parser::OnStartThread(wxCommandEvent& event)
{
	event.SetInt(0L);
	wxPostEvent(m_pParent, event);
}

void Parser::OnEndThread(wxCommandEvent& event)
{
	wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	m_ReparsedFiles.Remove(event.GetString());
	ParserThread* thread = (ParserThread*)event.GetInt();
	m_Threads.Remove(thread);
	delete lock;
	ScheduleThreads();

	if (!Done())
	{
		event.SetInt((int)this);
		wxPostEvent(m_pParent, event);
	}
}

void Parser::OnNewToken(wxCommandEvent& event)
{
#ifndef STANDALONE
	Manager::Get()->GetMessageManager()->DebugLog(event.GetString());
#endif
}

void Parser::OnParseFile(wxCommandEvent& event)
{
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
			tmp.Normalize(wxPATH_NORM_ALL, base);
			if (wxFileExists(tmp.GetFullPath()))
				break;
		}
	}

	fname.Normalize(wxPATH_NORM_ALL, base);
	//fname.Normalize(wxPATH_NORM_ALL, base); // the known wxWindows bug :(
	filename = fname.GetFullPath();

	/*wxMutexLocker* lock = new wxMutexLocker(s_mutexListProtection);
	bool abort = m_ReparsedFiles.Index(filename) != wxNOT_FOUND;
	delete lock;
	if (abort)
		return; // the file is being re-parsed; don't follow includes
	*/
	
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
	ClassTreeData* ctd = new ClassTreeData(token);
	int image = -1;
#ifndef STANDALONE
	image = GetTokenKindImage(token);
#endif
	wxString str = token->m_Name + token->m_Args;
	if (!token->m_ActualType.IsEmpty())
		 str = str + " : " + token->m_ActualType;
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
