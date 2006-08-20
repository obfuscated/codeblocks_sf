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
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "parserthread.h"
#include "parser.h"
#include <wx/app.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <globals.h>

#include <cctype>
#include <queue>

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
    const wxString implement_x(_T("IMPLEMENT_*"));
    const wxString unnamed(_T("Un-named"));
    const wxString declare_x(_T("DECLARE_*"));
    const wxString wx_declare_x(_T("WX_DECLARE_*"));
    const wxString wx_define_x(_T("WX_DEFINE_*"));
    const wxString quot(_T("\""));
    const wxString kw_C(_T("\"C\""));
    const wxString kw__asm(_T("__asm"));
    const wxString kw_class(_T("class"));
    const wxString kw_const(_T("const"));
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

ParserThread::ParserThread(Parser* parent,
							const wxString& bufferOrFilename,
							bool isLocal,
							ParserThreadOptions& options,
							TokensTree* tree)
	: m_pParent(parent),
	m_pTokens(tree),
	m_pLastParent(0),
	m_File(0),
	m_IsLocal(isLocal),
	m_Options(options),
	m_IsBuffer(options.useBuffer),
	m_Buffer(bufferOrFilename)
{
	//ctor
	m_Tokenizer.m_Options.wantPreprocessor = options.wantPreprocessor;
	m_LastScope = tsUndefined;
}

ParserThread::~ParserThread()
{
	//dtor
}

void ParserThread::Log(const wxString& log)
{
    if(TestDestroy())
        return;
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
        if(TestDestroy())
            return '\0';
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
        if(TestDestroy())
            return;
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
        if(TestDestroy())
            return;
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
    if(TestDestroy())
        return false;
    if (!m_pTokens)
        return false;
	s_MutexProtection.Enter();
	m_pTokens->Clear();
	s_MutexProtection.Leave();
	m_Tokenizer.InitFromBuffer(buffer);
	if (!m_Tokenizer.IsOK())
		return false;

	m_Str.Clear();
	while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();

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
		else if (token==ParserConsts::kw_namespace)
		{
		    HandleNamespace();
		    m_Str.Clear();
		}
		else if (token==ParserConsts::kw_struct ||
                token==ParserConsts::kw_class)
		{
		    // do not skip namespace/struct/class blocks
		    SkipToOneOfChars(_T("{;"), false);
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
						token.Matches(ParserConsts::implement_x) ||
						token.Matches(ParserConsts::declare_x) ||
						token.Matches(ParserConsts::wx_declare_x) ||
						token.Matches(ParserConsts::wx_define_x) )
                    {
                        m_Tokenizer.GetToken(); // skip args
                    }
                    else if (peek==ParserConsts::dcolon)
                    {
    //                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                        m_EncounteredNamespaces.push(token);
                        m_Tokenizer.GetToken(); // eat ::
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

bool ParserThread::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result)
{
    if(TestDestroy())
        return false;

	m_Tokenizer.InitFromBuffer(buffer);
	if (!m_Tokenizer.IsOK())
		return false;

    result.Clear();
	m_Str.Clear();
	while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();

	while (1)
	{
        if (!m_pTokens || TestDestroy())
            return false;

		wxString token = m_Tokenizer.GetToken();
		if (token.IsEmpty())
			break;

		if (token==ParserConsts::kw_namespace)
		{
		    SkipToOneOfChars(ParserConsts::opbrace);
		}
		else if (token==ParserConsts::opbrace)
		{
		    SkipBlock();
		}
		else if (token==ParserConsts::kw_using)
		{
		    wxString peek = m_Tokenizer.PeekToken();
		    if (peek == ParserConsts::kw_namespace)
		    {
		        // ok
		        m_Tokenizer.GetToken(); // eat namespace
		        while (true) // support full namespaces
		        {
                    m_Str << m_Tokenizer.GetToken();
                    if (m_Tokenizer.PeekToken() == ParserConsts::dcolon)
                        m_Str << m_Tokenizer.GetToken();
                    else
                        break;
		        }
		        if (!m_Str.IsEmpty())
                    result.Add(m_Str);
                m_Str.Clear();
		    }
		    else
                SkipToOneOfChars(ParserConsts::semicolonclbrace);
		}
	}
	return true;
}

bool ParserThread::InitTokenizer()
{
    if (!m_Buffer.IsEmpty())
	{
		if (!m_IsBuffer)
		{
			m_Filename = m_Buffer;
			return m_Tokenizer.Init(m_Filename);
		}

        return m_Tokenizer.InitFromBuffer(m_Buffer);
	}
	return false;
}

bool ParserThread::Parse()
{
//    Manager::Get()->GetMessageManager()->DebugLog(_T("> parsing %s"),m_Filename.c_str());
    if (!InitTokenizer())
        return false;
    bool result = false;

    do
    {
        if (!m_pTokens || !m_Tokenizer.IsOK())
            break;

        if(!m_Options.useBuffer) // Parse a file
        {
            s_MutexProtection.Enter();
            m_File = m_pTokens->ReserveFileForParsing(m_Filename);
            s_MutexProtection.Leave();
            if(!m_File)
                break;
        }

        DoParse();

        if(!m_Options.useBuffer) // Parsing a file
        {
            s_MutexProtection.Enter();
            m_pTokens->FlagFileAsParsed(m_Filename);
            s_MutexProtection.Leave();
        }
        result = true;
    }while(false);

    return result;
}

void ParserThread::DoParse()
{
	m_Str.Clear();
	m_LastToken.Clear();
	while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();
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
		else if (token==ParserConsts::kw_typedef)
		{
		    HandleTypedef();
		    m_Str.Clear();
		}
		else if (token==ParserConsts::kw_return ||
			token==ParserConsts::colon)
		{
			SkipToOneOfChars(ParserConsts::semicolonclbrace, true);
			m_Str.Clear();
		}
		else if (token==ParserConsts::kw_const)
		{
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
			if (m_Tokenizer.PeekToken() == ParserConsts::lt) // should be!
                SkipAngleBraces();
//			SkipToOneOfChars(ParserConsts::gtsemicolon, true);
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
							token.Matches(ParserConsts::implement_x) ||
							token.Matches(ParserConsts::declare_x) ||
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
				else if (peek==ParserConsts::colon && token != ParserConsts::kw_private &&
                                                      token != ParserConsts::kw_protected &&
                                                      token != ParserConsts::kw_public)
				{
                    // example decl to encounter a colon is when defining a bitfield: int x:1,y:1,z:1;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());
                    m_Tokenizer.GetToken(); // skip colon
                    m_Tokenizer.GetToken(); // skip bitfield
                    m_Tokenizer.GetToken(); // skip comma
				}
				else if (peek==ParserConsts::comma)
				{
                    // example decl to encounter a comma: int x,y,z;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());
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
//                        DBGLOG(_T("peek='::', token='") + token + _T("', m_LastToken='") + m_LastToken + _T("', m_Str='") + m_Str + _T("'"));
                        if (m_Str.IsEmpty())
                            m_EncounteredTypeNamespaces.push(token); // it's a type's namespace
                        else
                            m_EncounteredNamespaces.push(token);
                        m_Tokenizer.GetToken(); // eat ::
                    }
//                    if (m_Str.IsEmpty())
//                    {
////                        std::queue<wxString> q = m_EncounteredNamespaces;
//                        m_Str = GetQueueAsNamespaceString(m_EncounteredNamespaces) + token;
//                    }
				}
                else if (peek==ParserConsts::dcolon)
                {
//                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                    if (m_Str.IsEmpty())
                        m_EncounteredTypeNamespaces.push(token); // it's a type's namespace
                    else
                        m_EncounteredNamespaces.push(token);
                    m_Tokenizer.GetToken(); // eat ::
                }
				else if ((peek==ParserConsts::semicolon || (m_Options.useBuffer && peek.GetChar(0) == _T('(')) && !m_Str.Contains(ParserConsts::dcolon)) && m_pTokens)
				{
//					Log("m_Str='"+m_Str+"'");
//					Log("token='"+token+"'");
//					Log("peek='"+peek+"'");
					if (!m_Str.IsEmpty() && (isalpha(token.GetChar(0)) || token.GetChar(0) == '_'))
					{
                        DoAddToken(tkVariable, token, m_Tokenizer.GetLineNumber());
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
    Token* result;
    if (!m_pTokens)
        return 0;
    int parentidx = !parent ? -1 : parent->GetSelf();
    // no critical section needed here:
    // all functions that call this, already entered a critical section.
    result = m_pTokens->at(m_pTokens->TokenExists(name, parentidx, kindMask));
    return result;
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
	else
		return m_Str; // token ends at start of phrase
	return wxEmptyString; // never reaches here
}

wxString ParserThread::GetQueueAsNamespaceString(std::queue<wxString>& q)
{
    wxString result;
    while (!q.empty())
    {
        result << q.front() << ParserConsts::dcolon;
        q.pop();
    }
    return result;
}

Token* ParserThread::FindTokenFromQueue(std::queue<wxString>& q, Token* parent, bool createIfNotExist)
{
    if (q.empty())
        return 0;
    wxString ns = q.front();
    q.pop();

    Token* result = TokenExists(ns, parent, tkNamespace | tkClass);

    if (!result && createIfNotExist)
    {
        result = new Token(ns, 0, 0);
        result->m_TokenKind = tkNamespace;
        int newidx = m_pTokens->insert(result);
        if (parent)
        {
            parent->AddChild(newidx);
            result->m_ParentIndex = parent->GetSelf();
        }
    }

    if (q.empty())
        return result;
    if (result)
        result = FindTokenFromQueue(q, result, createIfNotExist);
    return result;
}

Token* ParserThread::DoAddToken(TokenKind kind, const wxString& name, int line, const wxString& args, bool isOperator, bool isImpl)
{
    if(TestDestroy())
        return 0;
	if (m_Options.useBuffer && TokenExists(name, m_pLastParent, kind))
		return 0;
    s_MutexProtection.Enter();
	Token* newToken = 0;
	wxString newname(name);
	m_Str.Trim();

	if (kind == tkDestructor)
	{
		// special class destructors case
		newname.Prepend(ParserConsts::tilde);
		m_Str.Clear();
	}

    Token* localParent = 0;

    // preserve m_EncounteredTypeNamespaces; needed further down this function
    std::queue<wxString> q = m_EncounteredTypeNamespaces;
	if (kind == tkDestructor || kind == tkConstructor && !q.empty())
	{
	    // look in m_EncounteredTypeNamespaces
        localParent = FindTokenFromQueue(q, 0, true);
        if (localParent)
            newToken = TokenExists(newname, localParent);
	}

    // check for implementation member function
    if (!newToken && !m_EncounteredNamespaces.empty())
    {
        localParent = FindTokenFromQueue(m_EncounteredNamespaces, 0, true);
        if (localParent)
            newToken = TokenExists(newname, localParent);
    }

    if(newToken)
    {
        m_pTokens->m_modified = true;
    }
    else
    {
        wxString readType = m_Str;
        wxString actualType = GetActualTokenType();
        if (actualType.Find(_T(' ')) == wxNOT_FOUND)
        {
            // token type must contain all namespaces
            actualType.Prepend(GetQueueAsNamespaceString(m_EncounteredTypeNamespaces));
        }

        Token* finalParent = localParent ? localParent : m_pLastParent;
        newToken = new Token(newname,m_File,line);
        newToken->m_Type = readType;
        newToken->m_ActualType = actualType;
        newToken->m_Args = args;
        newToken->m_Scope = m_LastScope;
        newToken->m_TokenKind = kind;
        newToken->m_IsLocal = m_IsLocal;
        newToken->m_IsTemp = m_Options.isTemp;
        newToken->m_ParentIndex = finalParent ? finalParent->GetSelf() : -1;
        newToken->m_IsOperator = isOperator;
    //    Log("Added token " +name+ ", type '" +newToken->m_Type+ "', actual '" +newToken->m_ActualType+ "'");
        int newidx = -1;
        newidx=m_pTokens->insert(newToken);
        if (finalParent)
            finalParent->AddChild(newidx);
    }

    if (isImpl)
    {
        newToken->m_ImplFile = m_File;
        newToken->m_ImplLine = line;
        m_pTokens->m_FilesMap[newToken->m_ImplFile].insert(newToken->GetSelf());
    }

    while (!m_EncounteredNamespaces.empty())
        m_EncounteredNamespaces.pop();
    while (!m_EncounteredTypeNamespaces.empty())
        m_EncounteredTypeNamespaces.pop();

    s_MutexProtection.Leave();
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
    if(TestDestroy())
        return;
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
		do
		{
            // setting all #includes as global
            // it's amazing how many projects use #include "..." for global headers (MSVC mainly - booh)
            isGlobal = true;

            if(!(isGlobal ? m_Options.followGlobalIncludes : m_Options.followLocalIncludes))
                break; // Nothing to do!

            wxString real_filename = m_pParent->GetFullFileName(m_Filename,filename,isGlobal);
            // Parser::GetFullFileName is thread-safe :)

            if(real_filename.IsEmpty())
                break; // File not found, do nothing.

            {
                wxCriticalSectionLocker lock(s_MutexProtection);
                if(m_pTokens->IsFileParsed(real_filename))
                    break; // Already being parsed elsewhere
            }

            // since we 'll be calling directly the parser's method, let's make it thread-safe
    		{
                wxCriticalSectionLocker lock2(s_mutexListProtection);
                m_pParent->OnParseFile(real_filename, isGlobal ? 1 : 0);
    		}
		}while(false);
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

		Token* newToken = DoAddToken(tkPreprocessor, token, m_Tokenizer.GetLineNumber());
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
    int line = m_Tokenizer.GetLineNumber();

    if (ns == ParserConsts::opbrace)
    {
        // parse inside anonymous namespace
        DoParse();
    }
    else
    {
        wxString next = m_Tokenizer.PeekToken(); // named namespace
        if (next==ParserConsts::opbrace)
        {
            // use the existing copy (if any)
            Token* newToken = TokenExists(ns, m_pLastParent, tkNamespace);
            if (!newToken)
                newToken = DoAddToken(tkNamespace, ns, line);
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
}

void ParserThread::HandleClass(bool isClass)
{
    int lineNr = m_Tokenizer.GetLineNumber();
	wxString ancestors;
	while (1)
	{
		wxString current = m_Tokenizer.GetToken(); // class name
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
//                DBGLOG(_T("Class '%s' has ancestors"), current.c_str());
				m_Tokenizer.GetToken(); // eat ":"
				while (1)
				{
					wxString tmp = m_Tokenizer.GetToken();
					next = m_Tokenizer.PeekToken();
					if (tmp==ParserConsts::kw_public ||
                        tmp==ParserConsts::kw_protected ||
                        tmp==ParserConsts::kw_private)
                    {
                        continue;
                    }

					if (!(tmp==ParserConsts::comma || tmp==ParserConsts::gt))
                    {
                        // fix for namespace usage in ancestors
                        if (tmp==ParserConsts::dcolon || next==ParserConsts::dcolon)
                            ancestors << tmp;
						else
                            ancestors << tmp << _T(',');
//						DBGLOG(_T("Adding ancestor ") + tmp);
                    }
					if (next.IsEmpty() ||
						next==ParserConsts::opbrace ||
						next==ParserConsts::semicolon)
                    {
						break;
                    }
                    else if (next==ParserConsts::lt)
                    {
                        // template class
                        m_Tokenizer.GetToken(); // reach "<"
                        SkipAngleBraces();
                    }
				}
//                DBGLOG(_T("Ancestors: ") + ancestors);
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
			else if (next==ParserConsts::opbrace)
			{
				Token* newToken = DoAddToken(tkClass, current, lineNr);
				if (!newToken)
					return;
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
    int lineNr = m_Tokenizer.GetLineNumber();
	wxString args = m_Tokenizer.GetToken();
    wxString peek = m_Tokenizer.PeekToken();
	if (!m_Str.StartsWith(ParserConsts::kw_friend))
	{
        bool isDtor = m_Str.StartsWith(ParserConsts::tilde);
        Token* localParent = 0;

	    if ((m_Str.IsEmpty() || isDtor) && !m_EncounteredTypeNamespaces.empty())
	    {
	        // probably a ctor/dtor
            std::queue<wxString> q = m_EncounteredTypeNamespaces; // preserve m_EncounteredTypeNamespaces; needed in DoAddToken()
            localParent = FindTokenFromQueue(q);
//            DBGLOG(_T("Ctor? '%s', m_Str='%s', localParent='%s'"), name.c_str(), m_Str.c_str(), localParent ? localParent->m_Name.c_str() : _T("<none>"));
	    }
	    else
	    {
            std::queue<wxString> q = m_EncounteredNamespaces; // preserve m_EncounteredNamespaces; needed in DoAddToken()
            localParent = FindTokenFromQueue(q);
	    }

		bool CtorDtor = m_pLastParent && name == m_pLastParent->m_Name;
		if (!CtorDtor)
            CtorDtor = localParent && name == localParent->m_Name;

		TokenKind kind = !CtorDtor ? tkFunction : (isDtor ? tkDestructor : tkConstructor);

//        Log("Adding function '"+name+"': m_Str='"+m_Str+"'"+", enc_ns="+(m_EncounteredNamespaces.GetCount()?m_EncounteredNamespaces[0]:"nil"));

		bool isImpl = false;
		while (!peek.IsEmpty()) // !eof
		{
            if (peek == ParserConsts::opbrace || // normal function
                peek == ParserConsts::colon) // probably a ctor with member initializers
            {
                isImpl = true;
                SkipToOneOfChars(ParserConsts::semicolonclbrace,true);
                break;
            }
            else if (peek == ParserConsts::clbrace || peek == ParserConsts::semicolon)
                break; // function decl
            else if (peek != ParserConsts::kw_const)
                break; // darned macros that do not end with a semicolon :/

            // if we reached here, eat the token so peek gets a new value
            m_Tokenizer.GetToken();
		    peek = m_Tokenizer.PeekToken();
		}
		DoAddToken(kind, name, lineNr, args, isOperator, isImpl);
	}
}

void ParserThread::HandleEnum()
{
	// enums have the following rough definition:
	// enum [xxx] { type1 name1 [= 1][, [type2 name2 [= 2]]] };
	bool isUnnamed = false;
	int lineNr = m_Tokenizer.GetLineNumber();
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
            newEnum = DoAddToken(tkEnum, token, lineNr);
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
                DoAddToken(tkEnumerator, token, m_Tokenizer.GetLineNumber());
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
//	// skip to ;
//	SkipToOneOfChars(ParserConsts::semicolon);
}

void ParserThread::HandleTypedef()
{
    // we will store typedefs as tkClass and put the typedef'd type as the class's
    // ancestor. this way, it will work through inheritance.
    // function pointers are a different beast and are handled differently.

    // this is going to be tough :(
    // let's see some examples:
    //
    // relatively easy:
    // typedef unsigned int uint32;
    // typedef std::map<String, StringVector> AnimableDictionaryMap;
    //
    // harder:
    // typedef void dMessageFunction (int errnum, const char *msg, va_list ap);
    //
    // even harder:
    // typedef void (*dMessageFunction)(int errnum, const char *msg, va_list ap);

	size_t lineNr = m_Tokenizer.GetLineNumber();
	bool is_function_pointer = false;
	std::queue<wxString> components;
	// get everything on the same line
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Typedef start"));
    wxString args;
    wxString token;
    wxString peek;
	while (true)
	{
	    token = m_Tokenizer.GetToken();
	    peek = m_Tokenizer.PeekToken();
	    if (token.IsEmpty() || token == ParserConsts::semicolon)
            break;

	    if (token == ParserConsts::kw_class ||
            token == ParserConsts::kw_struct ||
            token == ParserConsts::kw_enum)
	    {
	        // "typedef struct" is not supported
	        // "typedef class" is not supported
	        // "typedef enum" is not supported
	        SkipToOneOfChars(ParserConsts::semicolon, true);
            break;
	    }

        // skip templates <>
        if (peek == ParserConsts::lt)
        {
            SkipAngleBraces();
            continue;
        }

        // keep namespaces together
        while (peek == ParserConsts::dcolon)
        {
            token << peek;
            m_Tokenizer.GetToken(); // eat it
            token << m_Tokenizer.GetToken(); // get what's next
            peek = m_Tokenizer.PeekToken();

            if (peek == ParserConsts::lt)
                m_Tokenizer.UngetToken(); // or else templates check above will fail
        }

        if (token.GetChar(0) == '(')
        {
            // function pointer (probably)
            is_function_pointer = true;
            if (peek.GetChar(0) == '(')
            {
                // typedef void (*dMessageFunction)(int errnum, const char *msg, va_list ap);

                // remove parentheses and any dereferencing symbols
                token.RemoveLast();
                size_t pos = 1;
                while (pos < token.Length() && token.GetChar(pos) != '*')
                    pos++;
                while (token.GetChar(pos) == '*')
                    pos++;
                if (pos < token.Length())
                    token.Remove(0, pos); // remove up to *
                else
                    token.Remove(0, 1); // remove opening parenthesis
                args = peek;
                components.push(token);
            }
            else
            {
                // typedef void dMessageFunction (int errnum, const char *msg, va_list ap);

                // last component is already the name and this is the args
                args = token;
            }
            break;
        }

        components.push(token);
//        Manager::Get()->GetMessageManager()->DebugLog(_T(" + '%s'"), token.c_str());
	}
//    Manager::Get()->GetMessageManager()->DebugLog(_T("Typedef done"));

    if (!is_function_pointer && components.size() <= 1)
            return; // invalid typedef

    // now get the type
    wxString ancestor;
    while (components.size() > 1)
    {
        wxString token = components.front();
        components.pop();

        if (!ancestor.IsEmpty())
            ancestor << _T(' ');
        ancestor << token;
    }

    // no return type
    m_Str.Clear();

//    Manager::Get()->GetMessageManager()->DebugLog(_T("Adding typedef: name '%s', ancestor: '%s'"), components.front().c_str(), ancestor.c_str());
    Token* tdef = DoAddToken(tkClass, components.front(), lineNr, args);
    if (tdef)
    {
        tdef->m_IsTypedef = true;
        if (!is_function_pointer)
        {
            tdef->m_AncestorsString = ancestor;
            tdef->m_ActualType = ancestor;
        }
        else
            tdef->m_ActualType = ancestor + args;
    }
}
