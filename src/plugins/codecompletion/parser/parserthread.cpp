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
#include "parserthread.h"
#include <wx/app.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <globals.h>

#include <cctype>

int THREAD_START = wxNewId();
int THREAD_END = wxNewId();
int NEW_TOKEN = wxNewId();
int FILE_NEEDS_PARSING = wxNewId();

namespace ParserConsts
{
    const wxString space(_T(" "));
    const wxString spaced_colon(_T(" : "));
    const wxString empty(_T(""));
    const wxString hash(_T("#"));
    const wxString x_event_table(_T("*_EVENT_TABLE"));
    const wxString x_event_table_x(_T("*_EVENT_TABLE*"));
    const wxString plus(_T("+"));
    const wxString comma(_T(","));
    const wxString commaclbrace(_T(",}"));
    const wxString dash(_T("-"));
    const wxString dot(_T("."));
    const wxString colon(_T(":"));
    const wxString dcolon(_T("::"));
    const wxString semicolon(_T(";"));
    const wxString semicolonopbrace(_T(";{"));
    const wxString semicolonclbrace(_T(";}"));
    const wxString lt(_T("<"));
    const wxString gt(_T(">"));
    const wxString gtsemicolon(_T(">;"));
    const wxString begin_event_table(_T("BEGIN_EVENT_TABLE"));
    const wxString end_event_table(_T("END_EVENT_TABLE"));
    const wxString implement_app(_T("IMPLEMENT_APP"));
    const wxString implement_dynamic_class(_T("IMPLEMENT_DYNAMIC_CLASS"));
    const wxString unnamed(_T("Un-named"));
    const wxString wx_declare_x(_T("WX_DECLARE_*"));
    const wxString wx_define_x(_T("WX_DEFINE_*"));
    const wxString quot(_T("\""));
    const wxString kw_C(_T("\"C\""));
    const wxString kw__asm(_T("__asm"));
    const wxString kw_class(_T("class"));
    const wxString kw_define(_T("define"));
    const wxString kw_delete(_T("delete"));
    const wxString kw_do(_T("do"));
    const wxString kw_else(_T("else"));
    const wxString kw_enum(_T("enum"));
    const wxString kw_extern(_T("extern"));
    const wxString kw_for(_T("for"));
    const wxString kw_friend(_T("friend"));
    const wxString kw_if(_T("if"));
    const wxString kw_include(_T("include"));
    const wxString kw_inline(_T("inline"));
    const wxString kw_namespace(_T("namespace"));
    const wxString kw_operator(_T("operator"));
    const wxString kw_private(_T("private"));
    const wxString kw_protected(_T("protected"));
    const wxString kw_public(_T("public"));
    const wxString kw_return(_T("return"));
    const wxString kw_static(_T("static"));
    const wxString kw_struct(_T("struct"));
    const wxString kw_switch(_T("switch"));
    const wxString kw_template(_T("template"));
    const wxString kw_typedef(_T("typedef"));
    const wxString kw_union(_T("union"));
    const wxString kw_using(_T("using"));
    const wxString kw_virtual(_T("virtual"));
    const wxString kw_while(_T("while"));
    const wxString opbrace(_T("{"));
    const wxString opbracesemicolon(_T("{;"));
    const wxString clbrace(_T("}"));
    const wxString tilde(_T("~"));
};

ParserThread::ParserThread(wxEvtHandler* parent,bool* abortflag,
							const wxString& bufferOrFilename,
							bool isLocal,
							ParserThreadOptions& options,
							TokensTree* tree)
	: m_pParent(parent),
	m_pTokens(tree),
	m_pLastParent(0),
	m_File(0),
	m_IsLocal(isLocal),
	m_Options(options)
{
//	m_pAbort=abortflag;
	//ctor
	m_Tokenizer.m_Options.wantPreprocessor = options.wantPreprocessor;

	if (!bufferOrFilename.IsEmpty())
	{
		if (!options.useBuffer)
		{
			m_Filename = bufferOrFilename;
			m_Tokenizer.Init(m_Filename);
		}
		else
			m_Tokenizer.InitFromBuffer(bufferOrFilename);
	}
	m_LastScope = tsUndefined;
}

ParserThread::~ParserThread()
{
	//dtor
}

void ParserThread::Log(const wxString& log)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, NEW_TOKEN);
	event.SetString(log);
	event.SetInt(m_Tokenizer.GetLineNumber());
	wxPostEvent(m_pParent, event);
	Manager::ProcessPendingEvents();
}

void ParserThread::SetTokens(TokensTree* tokens)
{
    m_pTokens = tokens;
}

void* ParserThread::DoRun()
{
//	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, THREAD_START);
//    event.SetString(m_Filename);
//	event.SetInt((int)this);
//	wxPostEvent(m_pParent, event);
//
////    Log("ParserThread running for " + m_Filename);
//	Parse();
//
//	wxCommandEvent event1(wxEVT_COMMAND_MENU_SELECTED, THREAD_END);
//	event1.SetString(m_Filename);
//	event1.SetInt((int)this);
//	wxPostEvent(m_pParent, event1);

	return 0L;
}

wxChar ParserThread::SkipToOneOfChars(const wxString& chars, bool supportNesting)
{
	unsigned int level = m_Tokenizer.GetNestingLevel();
	while (1)
	{
		wxString token = m_Tokenizer.GetToken();
		if (token.IsEmpty())
			return '\0'; // eof

		if (!supportNesting || m_Tokenizer.GetNestingLevel() == level)
		{
			wxChar ch = token.GetChar(0);
			if (chars.Find(ch) != wxNOT_FOUND)
				return ch;
		}
	}
}

void ParserThread::SkipBlock()
{
	// skip tokens until we reach }
	// block nesting is taken into consideration too ;)

	// this is the nesting level we start at
	// we subtract 1 because we 're already inside the block
	// (since we 've read the {)
	unsigned int level = m_Tokenizer.GetNestingLevel() - 1;
	while (1)
	{
		wxString token = m_Tokenizer.GetToken();
		if (token.IsEmpty())
			break; // eof

		// if we reach the initial nesting level, we are done
		if (level == m_Tokenizer.GetNestingLevel())
			break;
	}
}

void ParserThread::SkipAngleBraces()
{
    int nestLvl = 0;
    while (true)
    {
        wxString tmp = m_Tokenizer.GetToken();
        if (tmp==ParserConsts::lt)
            ++nestLvl;
        else if (tmp==ParserConsts::gt)
            --nestLvl;
        else if (tmp==ParserConsts::semicolon)
        {
        	// unget token - leave ; on the stack
        	m_Tokenizer.UngetToken();
            break;
        }
        else if (tmp.IsEmpty())
            break;
        if (nestLvl <= 0)
            break;
    }
}

bool ParserThread::ParseBufferForFunctions(const wxString& buffer)
{
    if (!m_pTokens)
        return false;
	m_pTokens->Clear();
	m_Tokenizer.InitFromBuffer(buffer);
	if (!m_Tokenizer.IsOK())
		return false;

	m_Str.Clear();
    m_EncounteredNamespaces.Clear();

	while (1)
	{
        if (!m_pTokens || TestDestroy())
            return false;

		wxString token = m_Tokenizer.GetToken();
		if (token.IsEmpty())
			break;
#if 0
	if (!m_Str.IsEmpty())
		Log(m_Str);
#endif
#if 0
	if (!token.IsEmpty())
		Log(token);
#endif

		if (token==ParserConsts::semicolon)
		{
			m_Str.Clear();
		}
		else if (token==ParserConsts::opbrace)
		{
			SkipBlock();
			m_Str.Clear();
		}
		else if (token==ParserConsts::clbrace)
		{
			m_Str.Clear();
		}
//		else if (token.Matches("::"))
//		{
//			m_Str.Clear();
//		}
		else if (token==ParserConsts::kw_typedef
		 ||
			token==ParserConsts::colon)
		{
			SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_extern ||
			token.StartsWith(ParserConsts::kw__asm))
		{
			SkipToOneOfChars(ParserConsts::semicolon);
			//m_Str.Clear();
		}
		else if (token==ParserConsts::hash)
		{
			m_Tokenizer.GetToken();
			m_Tokenizer.GetToken();
			m_Str.Clear();
		}
		else
		{
			wxString peek = m_Tokenizer.PeekToken();
			if (!peek.IsEmpty())
			{
				if (peek.GetChar(0) == '(')
				{
					// function
					// ignore some well-known wxWindows macros
					if (token==ParserConsts::begin_event_table)
					{
						// skip till after END_EVENT_TABLE
						while (!token.IsEmpty() && token!=ParserConsts::end_event_table)
							token = m_Tokenizer.GetToken(); // skip args
						m_Tokenizer.GetToken(); // skip args
					}
					else if ( token.Matches(ParserConsts::x_event_table) ||
						token==ParserConsts::implement_app ||
						token.Matches(ParserConsts::wx_declare_x) ||
						token.Matches(ParserConsts::wx_define_x) )
                    {
                        m_Tokenizer.GetToken(); // skip args
                    }
                    else
                    {
						if (m_Str.GetChar(0) == '~')
						{
                            token = _T('~') + token;
                            m_Str.Clear();
                        }
                        HandleFunction(token);
                        m_Str.Clear();
                    }
				}
				else
				{
					m_Str << token << _T(' ');
				}
			}
		}
	}
	return true;
}

bool ParserThread::Parse()
{
    wxCriticalSectionLocker* lock = 0;
    bool can_parse = false;
    if (!m_pTokens)
        return false;
	if (!m_Tokenizer.IsOK())
	{
		//Log("Cannot parse " + m_Filename);
		return false;
    }

    if(!m_Options.useBuffer) // Parse a file
    {
        lock = new wxCriticalSectionLocker(s_mutexProtection);
        m_File = m_pTokens->ReserveFileForParsing(m_Filename);
        delete lock;
		if(m_File)
		{
            can_parse = true;
//            Log("Parsing " + m_Filename);
		}
		else
            can_parse = false;
    }
    else
        can_parse = true;

    if(!can_parse)
        return false;
    DoParse();

    if(!m_Options.useBuffer) // Parsing a file
    {
        lock = new wxCriticalSectionLocker(s_mutexProtection);
        m_pTokens->FlagFileAsParsed(m_Filename);
        delete lock;
    }
    return true;
}

void ParserThread::DoParse()
{
	m_Str.Clear();
	m_LastToken.Clear();
    m_EncounteredNamespaces.Clear();
	while (1)
	{
		if (!m_pTokens || TestDestroy())
			break;

		wxString token = m_Tokenizer.GetToken();
		if (token.IsEmpty())
			break;
#if 0
	if (!m_Str.IsEmpty())
		Log(m_Str);
#endif
#if 0
	if (!token.IsEmpty())
		Log(token);
#endif

		if (token==ParserConsts::semicolon)
		{
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_delete ||
                token==ParserConsts::dot ||
				(token==ParserConsts::gt && m_LastToken==ParserConsts::dash))
		{
			m_Str.Clear();
			SkipToOneOfChars(ParserConsts::semicolonclbrace);
		}
		else if (token==ParserConsts::opbrace)
		{
			if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
				SkipBlock();
			m_Str.Clear();
		}
		else if (token==ParserConsts::clbrace)
		{
			m_pLastParent = 0L;
			m_LastScope = tsUndefined;
			m_Str.Clear();
			// the only time we get to find a } is when recursively called by e.g. HandleClass
			// we have to return now...
			if (!m_Options.useBuffer)
                break;
		}
		else if (token==ParserConsts::colon)
		{
			if (m_LastToken==ParserConsts::kw_public)
				m_LastScope = tsPublic;
			else if (m_LastToken==ParserConsts::kw_protected)
				m_LastScope = tsProtected;
			else if (m_LastToken==ParserConsts::kw_private)
				m_LastScope = tsPrivate;
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_while ||
                token==ParserConsts::kw_if ||
                token==ParserConsts::kw_do ||
                token==ParserConsts::kw_else ||
                token==ParserConsts::kw_for ||
                token==ParserConsts::kw_switch)
		{
			if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
				SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
			else
				m_Tokenizer.GetToken(); //skip args
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_typedef ||
			token==ParserConsts::kw_return ||
			token==ParserConsts::colon)
		{
			SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_extern)
		{
            // check for "C"
            m_Str = m_Tokenizer.GetToken();
            if (m_Str==ParserConsts::kw_C)
            {
                m_Tokenizer.GetToken(); // "eat" {
                DoParse(); // time for recursion ;)
            }
            else
                SkipToOneOfChars(ParserConsts::semicolon); // skip externs
//                m_Tokenizer.UngetToken(); // nope, return the token back...
            m_Str.Clear();
        }
        else if (token.StartsWith(ParserConsts::kw__asm))
		{
			SkipToOneOfChars(ParserConsts::semicolon, true);
			//m_Str.Clear();
		}
		else if (token==ParserConsts::kw_static ||
			token==ParserConsts::kw_virtual ||
			token==ParserConsts::kw_inline)
		{
			// do nothing (skip it)
			//m_Str.Clear();
		}
		else if (token==ParserConsts::hash)
		{
			token = m_Tokenizer.GetToken();
			if (token==ParserConsts::kw_include)
				HandleIncludes();
			else if (token==ParserConsts::kw_define)
				HandleDefines();
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_using) // using namespace ?
		{
            SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_namespace)
		{
			m_Str.Clear();
			HandleNamespace();
		}
		else if (token==ParserConsts::kw_template)
		{
			m_Str.Clear();
			SkipToOneOfChars(ParserConsts::gtsemicolon, true);
		}
		else if (token==ParserConsts::kw_class)
		{
			m_Str.Clear();
			HandleClass();
		}
		else if (token==ParserConsts::kw_struct)
		{
			m_Str.Clear();
			HandleClass(false);
		}
		else if (token==ParserConsts::kw_enum)
		{
			m_Str.Clear();
			HandleEnum();
		}
		else if (token==ParserConsts::kw_union)
        {
            SkipToOneOfChars(ParserConsts::opbracesemicolon);
//            if (m_Tokenizer.GetToken() == "{")
            {
                Token* oldparent = m_pLastParent;
                DoParse();
                m_Str.Clear();
                m_pLastParent = oldparent;
            }
        }
#if 1
		else if (token==ParserConsts::kw_operator)
		{
			wxString func = token;
			while (1)
			{
				token = m_Tokenizer.GetToken();
				if (!token.IsEmpty())
				{
					if (token.GetChar(0) == '(')
					{
						// check for operator()()
						wxString peek = m_Tokenizer.PeekToken();
						if (!peek.IsEmpty() && peek.GetChar(0) != '(')
							m_Tokenizer.UngetToken();
						else
							func << token;
						break;
					}
					else
						func << token;
				}
				else
					break;
			}
			HandleFunction(func, true);
			m_Str.Clear();
		}
#endif
		else
		{
			wxString peek = m_Tokenizer.PeekToken();
			if (!peek.IsEmpty())
			{
				if (peek.GetChar(0) == '(' && !m_Options.useBuffer)
				{
					if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
					{
						// function
						// ignore some well-known wxWindows macros
						if (token==ParserConsts::begin_event_table)
						{
							// skip till after END_EVENT_TABLE
							while (!token.IsEmpty() && token!=ParserConsts::end_event_table)
								token = m_Tokenizer.GetToken(); // skip args
							m_Tokenizer.GetToken(); // skip args
						}
						else if (token.Matches(ParserConsts::x_event_table_x) ||
							token==ParserConsts::implement_app ||
							token==ParserConsts::implement_dynamic_class ||
							token.Matches(ParserConsts::wx_declare_x) ||
							token.Matches(ParserConsts::wx_define_x))
                        {
							m_Tokenizer.GetToken(); // skip args
                        }
						else
						{
//                            Log("m_Str='"+m_Str+"'");
//                            Log("token='"+token+"'");
//                            Log("peek='"+peek+"'");
							HandleFunction(token);
						}
					}
					else
						m_Tokenizer.GetToken(); // eat args when parsing block
					m_Str.Clear();
				}
				else if (peek==ParserConsts::comma)
				{
                    // example decl to encounter a comma: int x,y,z;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    DoAddToken(tkVariable, token);
                    // skip comma (we had peeked it)
                    m_Tokenizer.GetToken();
				}
				else if (peek==ParserConsts::lt)
				{
                    // a template, e.g. someclass<void>::memberfunc
                    // we have to skip <>, so we 're left with someclass::memberfunc
                    SkipAngleBraces();
                    peek = m_Tokenizer.PeekToken();
                    if (peek==ParserConsts::dcolon)
                    {
    //                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                        m_EncounteredNamespaces.Add(token);
                        m_Tokenizer.GetToken(); // eat ::
                    }
                    if (m_Str.IsEmpty())
                    {
                        m_Str = GetStringFromArray(m_EncounteredNamespaces, ParserConsts::dcolon) + token;
                        m_EncounteredNamespaces.Clear();
                    }
				}
                else if (peek==ParserConsts::dcolon)
                {
//                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                    m_EncounteredNamespaces.Add(token);
                    m_Tokenizer.GetToken(); // eat ::
                }
				else if ((peek==ParserConsts::semicolon || (m_Options.useBuffer && peek.GetChar(0) == _T('(')) && !m_Str.Contains(ParserConsts::dcolon)) && m_pTokens)
				{
//					Log("m_Str='"+m_Str+"'");
//					Log("token='"+token+"'");
//					Log("peek='"+peek+"'");
					if (!m_Str.IsEmpty() && (isalpha(token.GetChar(0)) || token.GetChar(0) == '_'))
					{
                        DoAddToken(tkVariable, token);
                    }
					//m_Str.Clear();
				}
				else
				{
					m_Str << token << _T(' ');
				}
			}
		}
		m_LastToken = token;
	}
}

Token* ParserThread::TokenExists(const wxString& name, Token* parent, short int kindMask)
{
    if (!m_pTokens)
        return 0;
    int parentidx;
    if(!parent)
        parentidx = -1;
    else
        parentidx = parent->GetSelf();
    return m_pTokens->at(m_pTokens->TokenExists(name, parentidx, kindMask));
}

wxString ParserThread::GetActualTokenType()
{
    // we will compensate for spaces between
    // namespaces (e.g. NAMESPACE :: SomeType) wich is valid C++ construct
    // we 'll remove spaces that follow a semicolon
	int pos = 0;
	while (pos < (int)m_Str.Length())
	{
        if (m_Str.GetChar(pos) == ' ' &&
            (
                (pos > 0 && m_Str.GetChar(pos - 1) == ':') ||
                (pos < (int)m_Str.Length() - 1 && m_Str.GetChar(pos + 1) == ':')
            )
           )
        {
            m_Str.Remove(pos, 1);
        }
        else
            ++pos;
	}

	// m_Str contains the full text before the token's declaration
	// an example m_Str value would be: const wxString&
	// what we do here is locate the actual return value (wxString in this example)
	// it will be needed by code completion code ;)
	pos = m_Str.Length() - 1;
	// we walk m_Str backwards until we find a non-space character which also is
	// not * or &
	//                        const wxString&
	// in this example, we would stop here ^
    while (pos >= 0 &&
            (isspace(m_Str.GetChar(pos)) ||
            m_Str.GetChar(pos) == '*' ||
            m_Str.GetChar(pos) == '&'))
        --pos;
	if (pos >= 0)
	{
		// we have the end of the word we 're interested in
        int end = pos;
		// continue walking backwards until we find the start of the word
		//                               const wxString&
		// in this example, we would stop here ^
        while (pos >= 0 && (isalnum(m_Str.GetChar(pos)) || m_Str.GetChar(pos) == '_' || m_Str.GetChar(pos) == ':'))
            --pos;
		return m_Str.Mid(pos + 1, end - pos);
	}
	return wxEmptyString;
}

Token* ParserThread::DoAddToken(TokenKind kind, const wxString& name, const wxString& args, bool isOperator)
{
	wxCriticalSectionLocker lock(s_mutexProtection);
	if (m_Options.useBuffer && TokenExists(name))
		return 0;
	Token* newToken = 0;
	wxString newname(name);
	m_Str.Trim();
	if (kind == tkDestructor)
	{
		// special class destructors case
		newname.Prepend(ParserConsts::tilde);
		m_Str.Clear();
	}

    // check for implementation member function
    size_t count = m_EncounteredNamespaces.GetCount();
    if (count)
    {
        Token* localParent = 0;
        size_t i = 0;
        do
        {
            localParent = TokenExists(m_EncounteredNamespaces[i], localParent, tkClass | tkNamespace);
            i++;
        }while(localParent && i < count);
        m_EncounteredNamespaces.Clear();
        if (localParent)
            newToken = TokenExists(newname, localParent);
    }

    if(newToken)
    {
        newToken->m_ImplFile = m_File;
        newToken->m_ImplLine = m_Tokenizer.GetLineNumber();
        m_pTokens->m_FilesMap[newToken->m_ImplFile].insert(newToken->GetSelf());
        m_pTokens->m_modified = true;
    }
    else
    {
        newToken = new Token(newname,m_File,m_Tokenizer.GetLineNumber());
        newToken->m_Type = m_Str;
        newToken->m_ActualType = GetActualTokenType();
        newToken->m_Args = args;
        newToken->m_Scope = m_LastScope;
        newToken->m_TokenKind = kind;
        newToken->m_IsLocal = m_IsLocal;
        newToken->m_ParentIndex = m_pLastParent ? (m_pLastParent->GetSelf()) : -1;
        newToken->m_ImplLine = 0;
        newToken->m_IsOperator = isOperator;
    //    Log("Added token " +name+ ", type '" +newToken->m_Type+ "', actual '" +newToken->m_ActualType+ "'");
        if (m_pLastParent)
            newToken->m_ParentName = m_pLastParent->m_Name;
        int newidx = -1;
        if (m_pTokens)
            newidx=m_pTokens->insert(newToken);
        if (m_pLastParent)
            m_pLastParent->AddChild(newidx);
    }
	return newToken;
}

void ParserThread::HandleIncludes()
{
	wxString filename;
	bool isGlobal = !m_IsLocal;
	wxString token = m_Tokenizer.GetToken();
	// now token holds something like:
	// "someheader.h"
	// < and will follow iostream.h, >
	if (!token.IsEmpty())
	{
		if (token.GetChar(0) == '"')
		{
			// "someheader.h"
			token.Replace(_T("\""), _T(""));
			filename = token;
		}
		else if (token.GetChar(0) == '<')
		{
			isGlobal = true;
			// next token is filename, next is . (dot), next is extension
			// basically we 'll loop until >
			while (1)
			{
				token = m_Tokenizer.GetToken();
				if (token.IsEmpty())
					break;
				if (token.GetChar(0) != '>')
					filename << token;
				else
					break;
			}
		}
	}

	if (!filename.IsEmpty())
	{
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, FILE_NEEDS_PARSING);
		event.SetString(m_Filename + _T("+") + filename);
		// setting all #includes as global
		// it's amazing how many projects use #include "..." for global headers (MSVC mainly - booh)
		event.SetInt(1);//isGlobal);
//		wxPostEvent(m_pParent, event);

        // since we 'll be calling directly the parser's method, let's make it thread-safe
		static wxMutex lock;
		wxMutexLocker l(lock);
		m_pParent->ProcessEvent(event);
	}
}

void ParserThread::HandleDefines()
{
	wxString filename;
	wxString token = m_Tokenizer.GetToken();
	m_Str.Clear();
	// now token holds something like:
	// BLAH_BLAH
	if (!token.IsEmpty())
	{
        // make sure preprocessor definitions are not going under namespaces or classes!
        Token* oldParent = m_pLastParent;
        m_pLastParent = 0L;

		Token* newToken = DoAddToken(tkPreprocessor, token);
		if (newToken)
            newToken->m_Line -= 1; // preprocessor definitions need correction for the line number
		if (m_Tokenizer.PeekToken().GetChar(0) == '(') // TODO: find better way...
			m_Tokenizer.GetToken(); // eat args

        m_pLastParent = oldParent;
	}
}

void ParserThread::HandleNamespace()
{
    wxString ns = m_Tokenizer.GetToken();
    wxString next = m_Tokenizer.PeekToken();

    if (next==ParserConsts::opbrace)
    {
        // use the existing copy (if any)
        Token* newToken = TokenExists(ns, 0, tkNamespace);
        if (!newToken)
            newToken = DoAddToken(tkNamespace, ns);
        if (!newToken)
            return;

        m_Tokenizer.GetToken(); // eat {

        Token* lastParent = m_pLastParent;
        TokenScope lastScope = m_LastScope;

        m_pLastParent = newToken;
        // default scope is: public for namespaces (actually no, but emulate it)
        m_LastScope = tsPublic;

        DoParse();

        m_pLastParent = lastParent;
        m_LastScope = lastScope;
    }
    else
        SkipToOneOfChars(ParserConsts::semicolonopbrace); // some kind of error in code ?
}

void ParserThread::HandleClass(bool isClass)
{
    int lineNr = m_Tokenizer.GetLineNumber();
	wxString ancestors;
	while (1)
	{
		wxString current = m_Tokenizer.GetToken();
		wxString next = m_Tokenizer.PeekToken();
		if (!current.IsEmpty() && !next.IsEmpty())
		{
			if (next==ParserConsts::lt) // template specialization
			{
                SkipAngleBraces();
                next = m_Tokenizer.PeekToken();
			}

			if (next==ParserConsts::colon) // has ancestor(s)
			{
                //Log("Class " + current + " has ancestors");
				m_Tokenizer.GetToken(); // eat ":"
				while (1)
				{
					wxString tmp = m_Tokenizer.GetToken();
					next = m_Tokenizer.PeekToken();
					if (!(
                        tmp==ParserConsts::comma ||
                        tmp==ParserConsts::gt ||
                        tmp==ParserConsts::kw_public ||
                        tmp==ParserConsts::kw_protected ||
                        tmp==ParserConsts::kw_private
                        ))
                    {
                        // fix for namespace usage in ancestors
                        if (tmp==ParserConsts::dcolon || next==ParserConsts::dcolon)
                            ancestors << tmp;
						else
                            ancestors << tmp << _T(',');
						//Log("Adding ancestor " + tmp);
                    }
					if (next.IsEmpty() ||
						next==ParserConsts::opbrace ||
						next==ParserConsts::semicolon)
						break;
                    else if (next==ParserConsts::lt)
                    {
                        // template class
                        int nest = 0;
                        m_Tokenizer.GetToken(); // eat "<"
                        while (1)
                        {
                            wxString tmp1 = m_Tokenizer.GetToken();
                            if (tmp1==ParserConsts::lt)
                                ++nest;
                            else if (tmp1==ParserConsts::gt)
                                --nest;

                            if (tmp1.IsEmpty() ||
                                tmp1==ParserConsts::opbrace ||
                                tmp1==ParserConsts::semicolon ||
                                (tmp1==ParserConsts::gt && nest <= 0))
                            {
                                m_Tokenizer.UngetToken(); // put it back before exiting
                                break;
                            }
                        }
                    }
				}
                //Log("Ancestors: " + ancestors);
			}

			if (current==ParserConsts::opbrace) // unnamed class/struct
			{
				Token* lastParent = m_pLastParent;
				TokenScope lastScope = m_LastScope;

				// default scope is: private for classes, public for structs
				m_LastScope = isClass ? tsPrivate : tsPublic;

				DoParse();

				m_pLastParent = lastParent;
				m_LastScope = lastScope;
                break;
			}
			else if (next==ParserConsts::opbrace)   // no ancestor(s)
			{
				Token* newToken = DoAddToken(tkClass, current);
				if (!newToken)
					return;
                newToken->m_Line = lineNr; // correct line number (might be messed if class has ancestors)
				newToken->m_AncestorsString = ancestors;

                m_Tokenizer.GetToken(); // eat {

				Token* lastParent = m_pLastParent;
				TokenScope lastScope = m_LastScope;

				m_pLastParent = newToken;
				// default scope is: private for classes, public for structs
				m_LastScope = isClass ? tsPrivate : tsPublic;

				DoParse();

				m_pLastParent = lastParent;
				m_LastScope = lastScope;
                break;
			}
			else if (next==ParserConsts::semicolon) // forward decl; we don't care
				break;
			else if (next.GetChar(0) == '(') // function: struct xyz& DoSomething()...
			{
				HandleFunction(current);
				break;
			}
		}
		else
			break;
	}
}

void ParserThread::HandleFunction(const wxString& name, bool isOperator)
{
    //Log("Adding function '"+name+"': m_Str='"+m_Str+"'");
	wxString args = m_Tokenizer.GetToken();
	if (!m_Str.StartsWith(ParserConsts::kw_friend))
	{
		TokenKind kind = tkFunction;
		bool CtorDtor = m_pLastParent && name.Matches(m_pLastParent->m_Name);
		if (!CtorDtor)
		{
            // check for m_EncounteredNamespaces
            unsigned int count = m_EncounteredNamespaces.GetCount();
            if (count)
            {
                Token* localParent = 0;
                for (unsigned int i = 0; i < count; ++i)
                {
                    localParent = TokenExists(m_EncounteredNamespaces[i], localParent, tkClass | tkNamespace);
                    if (!localParent)
                        break;
                }
                CtorDtor = localParent && name.Matches(localParent->m_Name);
            }
		}

		if (CtorDtor)
		{
			m_Str.Trim();
			if (m_Str.IsEmpty())
				kind = tkConstructor;
			else if (m_Str==ParserConsts::tilde)
				kind = tkDestructor;
		}
//        Log("Adding function '"+name+"': m_Str='"+m_Str+"'"+", enc_ns="+(m_EncounteredNamespaces.GetCount()?m_EncounteredNamespaces[0]:"nil"));
		DoAddToken(kind, name, args, isOperator);
	}
	if (!m_Tokenizer.PeekToken()==ParserConsts::clbrace)
		SkipToOneOfChars(ParserConsts::semicolonclbrace,true);
}

void ParserThread::HandleEnum()
{
	// enums have the following rough definition:
	// enum [xxx] { type1 name1 [= 1][, [type2 name2 [= 2]]] };
	bool isUnnamed = false;
	wxString token = m_Tokenizer.GetToken();
	if (token.IsEmpty())
		return;
    else if (token==ParserConsts::opbrace)
	{
        // we have an un-named enum
		token = ParserConsts::unnamed;
		m_Tokenizer.UngetToken(); // return '{' back
		isUnnamed = true;
    }

	Token* newEnum = 0L;
	unsigned int level = 0;
	if (isalpha(token.GetChar(0)))
	{
		if (m_Tokenizer.PeekToken().GetChar(0) != '{')
			return;

        if (isUnnamed)
        {
            // for unnamed enums, look if we already have "Unnamed", so we don't
            // add a new one for every unnamed enum we encounter, in this scope...
            newEnum = TokenExists(token, m_pLastParent, tkEnum);
        }

        if (!newEnum) // either named or first unnamed enum
            newEnum = DoAddToken(tkEnum, token);
		level = m_Tokenizer.GetNestingLevel();
		m_Tokenizer.GetToken(); // skip {
	}
	else
	{
		if (token.GetChar(0) != '{')
			return;
		level = m_Tokenizer.GetNestingLevel() - 1; // we 've already entered the { block
	}

	while (1)
	{
		// process enumerators
		token = m_Tokenizer.GetToken();
		wxString peek = m_Tokenizer.PeekToken();
		if (token.IsEmpty() || peek.IsEmpty())
			return; //eof
		if (token==ParserConsts::clbrace && level == m_Tokenizer.GetNestingLevel())
			break;
		// assignments (=xxx) are ignored by the tokenizer,
		// so we don't have to worry about them here ;)
		if (peek==ParserConsts::comma || peek==ParserConsts::clbrace || peek==ParserConsts::colon)
		{
            // this "if", avoids non-valid enumerators
            // like a comma (if no enumerators follow)
            if (isalpha(token.GetChar(0)))
            {
                Token* lastParent = m_pLastParent;
                m_pLastParent = newEnum;
                DoAddToken(tkEnumerator, token);
                m_pLastParent = lastParent;
			}
			if (peek==ParserConsts::colon)
			{
				// bit specifier (eg, xxx:1)
				//walk to , or }
				SkipToOneOfChars(ParserConsts::commaclbrace);
			}
		}
	}
	// skip to ;
	token = m_Tokenizer.GetToken();
	while (!token.IsEmpty() && token!=ParserConsts::semicolon)
		token = m_Tokenizer.GetToken();
}
