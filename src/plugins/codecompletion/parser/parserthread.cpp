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

#include "parserthread.h"
#include <wx/app.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <globals.h>

int THREAD_START = wxNewId();
int THREAD_END = wxNewId();
int NEW_TOKEN = wxNewId();
int FILE_NEEDS_PARSING = wxNewId();

ParserThread::ParserThread(wxEvtHandler* parent,bool* abortflag,
							const wxString& bufferOrFilename,
							bool isLocal,
							ParserThreadOptions& options,
							TokensArray* tokens)
	: m_pParent(parent),
	m_pTokens(tokens),
	m_pLastParent(0L),
	m_IsLocal(isLocal),
	m_StartBlockIndex(0),
	m_Options(options)
{
	m_pAbort=abortflag;
	//ctor
	m_Tokens.m_Options.wantPreprocessor = options.wantPreprocessor;
	
	if (!bufferOrFilename.IsEmpty())
	{
		if (!options.useBuffer)
		{
			m_Filename = bufferOrFilename;
			m_Tokens.Init(m_Filename);
		}
		else
			m_Tokens.InitFromBuffer(bufferOrFilename);
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
	event.SetInt(m_Tokens.GetLineNumber());
	wxPostEvent(m_pParent, event);
	wxYield();
}

void ParserThread::SetTokens(TokensArray* tokens)
{
    m_pTokens = tokens;
}

void* ParserThread::DoRun()
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, THREAD_START);
    event.SetString(m_Filename);
	event.SetInt((int)this);
	wxPostEvent(m_pParent, event);

//    Log("ParserThread running for " + m_Filename);
	Parse();

	wxCommandEvent event1(wxEVT_COMMAND_MENU_SELECTED, THREAD_END);
	event1.SetString(m_Filename);
	event1.SetInt((int)this);
	wxPostEvent(m_pParent, event1);

	return 0L;
}

wxChar ParserThread::SkipToOneOfChars(const wxString& chars, bool supportNesting)
{
	unsigned int level = m_Tokens.GetNestingLevel();
	while (1)
	{
		wxString token = m_Tokens.GetToken();
		if (token.IsEmpty())
			return '\0'; // eof

		if (!supportNesting ||
			(supportNesting && m_Tokens.GetNestingLevel() == level))
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
	unsigned int level = m_Tokens.GetNestingLevel() - 1;
	while (1)
	{
		wxString token = m_Tokens.GetToken();
		if (token.IsEmpty())
			break; // eof

		// if we reach the initial nesting level, we are done
		if (level == m_Tokens.GetNestingLevel())
			break;
	}
}

void ParserThread::SkipAngleBraces()
{
    int nestLvl = 0;
    while (true)
    {
        wxString tmp = m_Tokens.GetToken();
        if (tmp.Matches("<"))
            ++nestLvl;
        else if (tmp.Matches(">"))
            --nestLvl;
        else if (tmp.IsEmpty() || tmp.Matches(";"))
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
	m_Tokens.InitFromBuffer(buffer);
	if (!m_Tokens.IsOK())
		return false;

	m_Str.Clear();
    m_EncounteredNamespaces.Clear();
	
	while (1)
	{
        if (!m_pTokens || TestDestroy())
            return false;

		wxString token = m_Tokens.GetToken();
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

		if (token.Matches(";"))
		{
			m_Str.Clear();
		}
		else if (token.Matches("{"))
		{
			SkipBlock();
			m_Str.Clear();
		}
		else if (token.Matches("}"))
		{
			m_Str.Clear();
		}
//		else if (token.Matches("::"))
//		{
//			m_Str.Clear();
//		}
		else if (token.Matches("typedef") ||
			token.Matches(":"))
		{
			SkipToOneOfChars(";}", true);
			m_Str.Clear();
		}
		else if (token.Matches("extern") ||
			token.StartsWith("__asm"))
		{
			SkipToOneOfChars(";");
			//m_Str.Clear();
		}
		else if (token.Matches("#"))
		{
			m_Tokens.GetToken();
			m_Tokens.GetToken();
			m_Str.Clear();
		}
		else
		{
			wxString peek = m_Tokens.PeekToken();
			if (!peek.IsEmpty())
			{
				if (peek.GetChar(0) == '(')
				{
					// function
					// ignore some well-known wxWindows macros
					if (token.Matches("BEGIN_EVENT_TABLE"))
					{
						// skip till after END_EVENT_TABLE
						while (!token.IsEmpty() && !token.Matches("END_EVENT_TABLE"))
							token = m_Tokens.GetToken(); // skip args
						m_Tokens.GetToken(); // skip args
					}
					else if (!token.Matches("*_EVENT_TABLE") &&
						!token.Matches("IMPLEMENT_APP") &&
						!token.Matches("WX_DECLARE_*") &&
						!token.Matches("WX_DEFINE_*"))
                    {
						if (m_Str.GetChar(0) == '~')
						{
                            token = '~' + token;
                            m_Str.Clear();
                        }
                        HandleFunction(token);
                    }
					else
						m_Tokens.GetToken(); // skip args
				}
				else
				{
					m_Str << token << " ";
				}
			}
		}
	}
	return true;
}

bool ParserThread::Parse()
{
    if (!m_pTokens)
        return false;
#if 0
	if (!m_Options.useBuffer)
		Log("Parsing " + m_Filename); 
#endif

	if (!m_Tokens.IsOK())
	{
		//Log("Cannot parse " + m_Filename); 
		return false;
    }

	if (m_Options.useBuffer)
		m_StartBlockIndex = m_pTokens->GetCount();
    else
        m_StartBlockIndex = 0;
	m_Str.Clear();
	m_LastToken.Clear();
    m_EncounteredNamespaces.Clear();

	while (1)
	{
		if (!m_pTokens || TestDestroy())
			break;
		
		wxString token = m_Tokens.GetToken();
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

		if (token.Matches(";"))
		{
			m_Str.Clear();
		}
		else if (token.Matches("delete") ||
                token.Matches(".") ||
				(token.Matches(">") && m_LastToken.Matches("-")))
		{
			m_Str.Clear();
			SkipToOneOfChars(";}");
		}
		else if (token.Matches("{"))
		{
			if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
				SkipBlock();
			m_Str.Clear();
		}
		else if (token.Matches("}"))
		{
			m_pLastParent = 0L;
			m_LastScope = tsUndefined;
			m_Str.Clear();
			// the only time we get to find a } is when recursively called by e.g. HandleClass
			// we have to return now...
			if (!m_Options.useBuffer)
                break;
		}
		else if (token.Matches(":"))
		{
			if (m_LastToken.Matches("public"))
				m_LastScope = tsPublic;
			else if (m_LastToken.Matches("protected"))
				m_LastScope = tsProtected;
			else if (m_LastToken.Matches("private"))
				m_LastScope = tsPrivate;
			m_Str.Clear();
		}
		else if (token.Matches("while") ||
			token.Matches("if") ||
			token.Matches("do") ||
			token.Matches("else") ||
			token.Matches("for") ||
			token.Matches("switch"))
		{
			if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
				SkipToOneOfChars(";}", true);
			else
				m_Tokens.GetToken(); //skip args
			m_Str.Clear();
		}
		else if (token.Matches("typedef") ||
			token.Matches("return") ||
			token.Matches(":"))
		{
			SkipToOneOfChars(";}", true);
			m_Str.Clear();
		}
		else if (token.Matches("extern"))
		{
            // check for "C"
            m_Str = m_Tokens.GetToken();
            if (m_Str.Matches("\"C\""))
            {
                m_Tokens.GetToken(); // "eat" {
                Parse(); // time for recursion ;)
            }
            else
                SkipToOneOfChars(";"); // skip externs
//                m_Tokens.UngetToken(); // nope, return the token back...
            m_Str.Clear();
        }
        else if (token.StartsWith("__asm"))
		{
			SkipToOneOfChars(";", true);
			//m_Str.Clear();
		}
		else if (token.Matches("static") ||
			token.Matches("virtual") ||
			token.Matches("inline"))
		{
			// do nothing (skip it)
			//m_Str.Clear();
		}
		else if (token.Matches("#"))
		{
			token = m_Tokens.GetToken();
			if (token.Matches("include"))
				HandleIncludes();
			else if (token.Matches("define"))
				HandleDefines();
			m_Str.Clear();
		}
		else if (token.Matches("using")) // using namespace ?
		{
            SkipToOneOfChars(";}", true);
			m_Str.Clear();
		}
		else if (token.Matches("namespace"))
		{
			m_Str.Clear();
			HandleNamespace();
		}
		else if (token.Matches("template"))
		{
			m_Str.Clear();
			SkipToOneOfChars(">;", true);
		}
		else if (token.Matches("class"))
		{
			m_Str.Clear();
			HandleClass();
		}
		else if (token.Matches("struct"))
		{
			m_Str.Clear();
			HandleClass(false);
		}
		else if (token.Matches("enum"))
		{
			m_Str.Clear();
			HandleEnum();
		}
		else if (token.Matches("union"))
        {
            SkipToOneOfChars("{;");
//            if (m_Tokens.GetToken() == "{")
            {
                Token* oldparent = m_pLastParent;
                Parse();
                m_Str.Clear();
                m_pLastParent = oldparent;
            }
        }
#if 1
		else if (token.Matches("operator"))
		{
			wxString func = token;
			while (1)
			{
				token = m_Tokens.GetToken();
				if (!token.IsEmpty())
				{
					if (token.GetChar(0) == '(')
					{
						// check for operator()()
						wxString peek = m_Tokens.PeekToken();
						if (!peek.IsEmpty() && peek.GetChar(0) != '(')
							m_Tokens.UngetToken();
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
			wxString peek = m_Tokens.PeekToken();
			if (!peek.IsEmpty())
			{
				if (peek.GetChar(0) == '(' && !m_Options.useBuffer)
				{
					if (!m_Options.useBuffer || m_Options.bufferSkipBlocks)
					{
						// function
						// ignore some well-known wxWindows macros
						if (token.Matches("BEGIN_EVENT_TABLE"))
						{
							// skip till after END_EVENT_TABLE
							while (!token.IsEmpty() && !token.Matches("END_EVENT_TABLE"))
								token = m_Tokens.GetToken(); // skip args
							m_Tokens.GetToken(); // skip args
						}
						else if (!token.Matches("*_EVENT_TABLE*") &&
							!token.Matches("IMPLEMENT_APP") &&
							!token.Matches("IMPLEMENT_DYNAMIC_CLASS") &&
							!token.Matches("WX_DECLARE_*") &&
							!token.Matches("WX_DEFINE_*"))
                        {
//                            Log("m_Str='"+m_Str+"'");
//                            Log("token='"+token+"'");
//                            Log("peek='"+peek+"'");
							HandleFunction(token);
                        }
						else
							m_Tokens.GetToken(); // skip args
					}
					else
						m_Tokens.GetToken(); // eat args when parsing block
					m_Str.Clear();
				}
				else if (peek.Matches(","))
				{
                    // example decl to encounter a comma: int x,y,z;
                    // token should hold the var (x/y/z)
                    // m_Str should hold the type (int)
                    DoAddToken(tkVariable, token);
                    // skip comma (we had peeked it)
                    m_Tokens.GetToken();
				}
				else if (peek.Matches("<"))
				{
                    // a template, e.g. someclass<void>::memberfunc
                    // we have to skip <>, so we 're left with someclass::memberfunc
                    SkipAngleBraces();
                    peek = m_Tokens.PeekToken();
                    if (peek.Matches("::"))
                    {
    //                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                        m_EncounteredNamespaces.Add(token);
                        m_Tokens.GetToken(); // eat ::
                    }
                    if (m_Str.IsEmpty())
                    {
                        m_Str = GetStringFromArray(m_EncounteredNamespaces, "::") + token;
                        m_EncounteredNamespaces.Clear();
                    }
				}
                else if (peek.Matches("::"))
                {
//                    Log("peek='::', token='" + token + "', m_LastToken='" + m_LastToken + "', m_Str='" + m_Str + "'");
                    m_EncounteredNamespaces.Add(token);
                    m_Tokens.GetToken(); // eat ::
                }
				else if ((peek.Matches(";") || (m_Options.useBuffer && peek.GetChar(0) == '(') && !m_Str.Contains("::")) && m_pTokens)
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
					m_Str << token << " ";
				}
			}
		}
		m_LastToken = token;
	}
	return true;
}

Token* ParserThread::TokenExists(const wxString& name, Token* parent, short int kindMask)
{
    if (!m_pTokens)
        return 0;
    if (!parent)
    {
        // when parsing a block, we must make sure the token does not already exist...
        for (unsigned int i = m_StartBlockIndex; i < m_pTokens->GetCount(); ++i)
        {
            Token* token = m_pTokens->Item(i);
            if ((token->m_TokenKind & kindMask) && token->m_Name.Matches(name))
                return token;
        }
    }
    else
    {
        // search only under the parent token
        for (unsigned int i = 0; i < parent->m_Children.GetCount(); ++i)
        {
            Token* token = parent->m_Children.Item(i);
            if ((token->m_TokenKind & kindMask) && token->m_Name.Matches(name))
                return token;
        }
    }
	return 0L;
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
	wxMutexLocker lock(s_mutexProtection);
	if (m_Options.useBuffer && TokenExists(name))
		return 0L;
	Token* newToken = new Token;
	m_Str.Trim();
	if (kind == tkDestructor)
	{
		// special class destructors case
		newToken->m_Name = "~" + name;
		m_Str.Clear();
	}
	else
		newToken->m_Name = name;

    // check for implementation member function
    Token* localParent = 0;
    if (m_EncounteredNamespaces.GetCount())
    {
        unsigned int count = m_EncounteredNamespaces.GetCount();
        for (unsigned int i = 0; i < count; ++i)
        {
//            Log("NS: '" + m_EncounteredNamespaces[i] + "' for " + newToken->m_Name);
            localParent = TokenExists(m_EncounteredNamespaces[i], localParent, tkClass | tkNamespace);
            if (!localParent)
                break;
        }
        m_EncounteredNamespaces.Clear();
    }
    if (localParent)
    {
//        Log("Parent found for " + m_Str + " " + newToken->m_Name + ": " + localParent->m_DisplayName);
        Token* existing = TokenExists(newToken->m_Name, localParent);
        if (existing)
        {
//            Log("Existing found for " + newToken->m_Name);
            // if the token exists, all we have to do is adjust the
            // implementation file/line
            existing->m_ImplFilename = m_Tokens.GetFilename();
            existing->m_ImplLine = m_Tokens.GetLineNumber();
            delete newToken;
            return existing;
        }
    }

	newToken->m_Type = m_Str;
	newToken->m_ActualType = GetActualTokenType();	
	newToken->m_Args = args;
	newToken->m_Scope = m_LastScope;
	newToken->m_TokenKind = kind;
	newToken->m_IsLocal = m_IsLocal;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_Line = m_Tokens.GetLineNumber();
	newToken->m_ImplLine = 0;
	newToken->m_IsOperator = isOperator;
	newToken->m_IsTemporary = m_Options.useBuffer;
//    Log("Added token " +name+ ", type '" +newToken->m_Type+ "', actual '" +newToken->m_ActualType+ "'");
	if (m_pLastParent)
		newToken->m_DisplayName << m_pLastParent->m_Name << "::";
	newToken->m_DisplayName << newToken->m_Name << args;
	if (!newToken->m_Type.IsEmpty())
		newToken->m_DisplayName << " : " << newToken->m_Type;

    if (m_pTokens)
        m_pTokens->Add(newToken);
    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);

	return newToken;
}

void ParserThread::HandleIncludes()
{
	wxString filename;
	bool isGlobal = !m_IsLocal;
	wxString token = m_Tokens.GetToken();
	// now token holds something like:
	// "someheader.h"
	// < and will follow iostream.h, >
	if (!token.IsEmpty())
	{
		if (token.GetChar(0) == '"')
		{
			// "someheader.h"
			token.Replace("\"", "");
			filename = token;
		}
		else if (token.GetChar(0) == '<')
		{
			isGlobal = true;
			// next token is filename, next is . (dot), next is extension
			// basically we 'll loop until >
			while (1)
			{
				token = m_Tokens.GetToken();
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
		event.SetString(m_Filename + "+" + filename);
		// setting all #includes as global
		// it's amazing how many projects use #include "..." for global headers (MSVC mainly - booh)
		event.SetInt(1);//isGlobal);
		wxPostEvent(m_pParent, event);
	}
}

void ParserThread::HandleDefines()
{
	wxString filename;
	wxString token = m_Tokens.GetToken();
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
		if (m_Tokens.PeekToken().GetChar(0) == '(') // TODO: find better way...
			m_Tokens.GetToken(); // eat args

        m_pLastParent = oldParent;
	}
}

void ParserThread::HandleNamespace()
{
    wxString ns = m_Tokens.GetToken();
    wxString next = m_Tokens.PeekToken();
    
    if (next.Matches("{"))
    {
        // use the existing copy (if any)
        Token* newToken = TokenExists(ns, 0, tkNamespace);
        if (!newToken)
            newToken = DoAddToken(tkNamespace, ns);
        if (!newToken)
            return;

        m_Tokens.GetToken(); // eat {
            
        Token* lastParent = m_pLastParent;
        TokenScope lastScope = m_LastScope;
        
        m_pLastParent = newToken;
        // default scope is: public for namespaces (actually no, but emulate it)
        m_LastScope = tsPublic;
        
        Parse();
        
        m_pLastParent = lastParent;
        m_LastScope = lastScope;
    }
    else
        SkipToOneOfChars(";{"); // some kind of error in code ?
}

void ParserThread::HandleClass(bool isClass)
{
    int lineNr = m_Tokens.GetLineNumber();
	wxString ancestors;
	while (1)
	{
		wxString current = m_Tokens.GetToken();
		wxString next = m_Tokens.PeekToken();
		if (!current.IsEmpty() && !next.IsEmpty())
		{
			if (next.Matches("<")) // template specialization
			{
                SkipAngleBraces();
                next = m_Tokens.PeekToken();
			}

			if (next.Matches(":")) // has ancestor(s)
			{
                //Log("Class " + current + " has ancestors");
				m_Tokens.GetToken(); // eat ":"
				while (1)
				{
					wxString tmp = m_Tokens.GetToken();
					next = m_Tokens.PeekToken();
					if (!tmp.Matches("public") &&
						!tmp.Matches("protected") &&
						!tmp.Matches("private") &&
						!tmp.Matches(">") &&
						!tmp.Matches(","))
                    {
                        // fix for namespace usage in ancestors
                        if (tmp.Matches("::") || next.Matches("::"))
                            ancestors << tmp;
						else
                            ancestors << tmp << ',';
						//Log("Adding ancestor " + tmp);
                    }
					if (next.IsEmpty() ||
						next.Matches("{") ||
						next.Matches(";"))
						break;
                    else if (next.Matches("<"))
                    {
                        // template class
                        int nest = 0;
                        m_Tokens.GetToken(); // eat "<"
                        while (1)
                        {
                            wxString tmp1 = m_Tokens.GetToken();
                            if (tmp1.Matches("<"))
                                ++nest;
                            else if (tmp1.Matches(">"))
                                --nest;
                                
                            if (tmp1.IsEmpty() ||
                                tmp1.Matches("{") ||
                                tmp1.Matches(";") ||
                                (tmp1.Matches(">") && nest <= 0))
                            {
                                m_Tokens.UngetToken(); // put it back before exiting
                                break;
                            }
                        }
                    }
				}
                //Log("Ancestors: " + ancestors);
			}
			
			if (current.Matches("{")) // unnamed class/struct
			{
				Token* lastParent = m_pLastParent;
				TokenScope lastScope = m_LastScope;

				// default scope is: private for classes, public for structs
				m_LastScope = isClass ? tsPrivate : tsPublic;
				
				Parse();
				
				m_pLastParent = lastParent;
				m_LastScope = lastScope;
                break;
			}
			else if (next.Matches("{"))   // no ancestor(s)
			{
				Token* newToken = DoAddToken(tkClass, current);
				if (!newToken)
					return;
                newToken->m_Line = lineNr; // correct line number (might be messed if class has ancestors)
				newToken->m_AncestorsString = ancestors;

                m_Tokens.GetToken(); // eat {
					
				Token* lastParent = m_pLastParent;
				TokenScope lastScope = m_LastScope;
				
				m_pLastParent = newToken;
				// default scope is: private for classes, public for structs
				m_LastScope = isClass ? tsPrivate : tsPublic;
				
				Parse();
				
				m_pLastParent = lastParent;
				m_LastScope = lastScope;
                break;
			}
			else if (next.Matches(";")) // forward decl; we don't care
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
	wxString args = m_Tokens.GetToken();
	if (!m_Str.StartsWith("friend"))
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
			else if (m_Str.Matches("~"))
				kind = tkDestructor;
		}
//        Log("Adding function '"+name+"': m_Str='"+m_Str+"'"+", enc_ns="+(m_EncounteredNamespaces.GetCount()?m_EncounteredNamespaces[0]:"nil"));
		DoAddToken(kind, name, args, isOperator);
	}
	if (!m_Tokens.PeekToken().Matches("}"))
		SkipToOneOfChars(";}", true);
}

void ParserThread::HandleEnum()
{
	// enums have the following rough definition:
	// enum [xxx] { type1 name1 [= 1][, [type2 name2 [= 2]]] };
	bool isUnnamed = false;
	wxString token = m_Tokens.GetToken();
	if (token.IsEmpty())
		return;
    else if (token.Matches("{"))
	{
        // we have an un-named enum
		token = "Un-named";
		m_Tokens.UngetToken(); // return '{' back
		isUnnamed = true;
    }
	
	Token* newEnum = 0L;
	unsigned int level = 0;
	if (isalpha(token.GetChar(0)))
	{
		if (m_Tokens.PeekToken().GetChar(0) != '{')
			return;

        if (isUnnamed)
        {
            // for unnamed enums, look if we already have "Unnamed", so we don't
            // add a new one for every unnamed enum we encounter, in this scope...
            newEnum = TokenExists(token, m_pLastParent, tkEnum);
        }

        if (!newEnum) // either named or first unnamed enum
            newEnum = DoAddToken(tkEnum, token);
		level = m_Tokens.GetNestingLevel();
		m_Tokens.GetToken(); // skip {
	}
	else
	{
		if (token.GetChar(0) != '{')
			return;
		level = m_Tokens.GetNestingLevel() - 1; // we 've already entered the { block
	}
	
	while (1)
	{
		// process enumerators
		token = m_Tokens.GetToken();
		wxString peek = m_Tokens.PeekToken();
		if (token.IsEmpty() || peek.IsEmpty())
			return; //eof
		if (token.Matches("}") && level == m_Tokens.GetNestingLevel())
			break;
		// assignments (=xxx) are ignored by the tokenizer,
		// so we don't have to worry about them here ;)
		if (peek.Matches(",") || peek.Matches("}") || peek.Matches(":"))
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
			if (peek.Matches(":"))
			{
				// bit specifier (eg, xxx:1)
				//walk to , or }
				SkipToOneOfChars(",}");
			}
		}
	}
	// skip to ;
	token = m_Tokens.GetToken();
	while (!token.IsEmpty() && !token.Matches(";"))
		token = m_Tokens.GetToken();
}
