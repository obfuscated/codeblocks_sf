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

#include "tokenizer.h"
#include <wx/utils.h>
#include <wx/file.h>
#include <wx/log.h>

Tokenizer::Tokenizer(const wxString& filename)
	: m_Filename(filename),
	m_BufferLen(0),
	m_NestLevel(0),
	m_UndoNestLevel(0),
	m_TokenIndex(0),
	m_UndoTokenIndex(0),
	m_LineNumber(1),
	m_UndoLineNumber(1),
	m_IsOK(false),
	m_IsOperator(false)
{
	//ctor
	m_Options.wantPreprocessor = false;
	if (!m_Filename.IsEmpty())
		Init(m_Filename);
}

Tokenizer::~Tokenizer()
{
	//dtor
}

bool Tokenizer::Init(const wxString& filename)
{
	BaseInit();
	if (filename.IsEmpty())
	{
		if (m_Filename.IsEmpty())
		{
			wxLogError("Tokenizer::Init() called without filename...");
			return false;
		}
	}
	else
		m_Filename = filename;

	if (!wxFileExists(m_Filename))
		return false;

	if (!ReadFile())
	{
		wxLogError("File %s does not exist...", m_Filename.c_str());
		return false;
	}

	if (!m_BufferLen)
	{
		//wxLogError("File is empty!");
		return false;
	}

	m_IsOK = true;
	return true;
}

bool Tokenizer::InitFromBuffer(const wxString& buffer)
{
	BaseInit();
	m_Buffer = buffer;
	m_BufferLen = buffer.Length();
	m_IsOK = true;
	m_Filename.Clear();
	return true;
}

void Tokenizer::BaseInit()
{
	m_TokenIndex = 0;
	m_UndoTokenIndex = 0;
	m_LineNumber = 1;
	m_UndoLineNumber = 1;
	m_NestLevel = 0;
	m_UndoNestLevel = 0;
	m_IsOperator = false;
	m_BufferLen = 0;
	m_LastWasPreprocessor = false;
	m_LastPreprocessor.Clear();
	m_Buffer.Clear();
	m_IsOK = false;
}

bool Tokenizer::ReadFile()
{
    if (!wxFileExists(m_Filename))
        return false;

    // open file
    wxFile file(m_Filename);

    if (!file.IsOpened())
        return false;

    char* buff = m_Buffer.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    m_Buffer.UngetWriteBuf();
	m_BufferLen = m_Buffer.Length();
	
    return true;
}

wxChar Tokenizer::CurrentChar()
{
	return m_Buffer.GetChar(m_TokenIndex);
}

wxChar Tokenizer::NextChar()
{
	if ((m_TokenIndex + 1) < 0 || (m_TokenIndex + 1) >= m_BufferLen)
		return 0;
	return m_Buffer.GetChar(m_TokenIndex + 1);
}

wxChar Tokenizer::PreviousChar()
{
	if ((m_TokenIndex - 1) < 0 || (m_TokenIndex - 1) >= m_BufferLen)
		return 0;
	return m_Buffer.GetChar(m_TokenIndex - 1);
}

void Tokenizer::AdjustLineNumber()
{
	if (CurrentChar() == '\n')
		++m_LineNumber;
}

bool Tokenizer::MoveToNextChar()
{
	++m_TokenIndex;
	if (!IsEOF())
	{
		AdjustLineNumber();
		return true;
	}
	return false;
}

bool Tokenizer::SkipWhiteSpace()
{
	// skip spaces, tabs, etc.
	while (!IsEOF() && isspace(CurrentChar()))
		MoveToNextChar();
	if (IsEOF())
		return false;
	return true;
}

bool Tokenizer::SkipToChar(const char& ch)
{
	// skip everything until we find ch
	while (1)
	{
		while (!IsEOF() && CurrentChar() != ch)
			MoveToNextChar();
		if (PreviousChar() != '\\')
			break;
		else
		{
			// check for "\\" 
			if (m_TokenIndex - 2 >= 0 && m_Buffer.GetChar(m_TokenIndex - 2) == '\\')
				break;
		}
		MoveToNextChar();
	}
	if (IsEOF())
		return false;
	return true;
}

bool Tokenizer::CharInString(const char ch, const char* chars)
{
	int len = strlen(chars);
	for (int i = 0; i < len; ++i)
	{
		if (ch == chars[i])
			return true;
	}
	return false;
}

bool Tokenizer::SkipToOneOfChars(const char* chars, bool supportNesting)
{
	// skip everything until we find any one of chars
	while (1)
	{
		while (!IsEOF() && !CharInString(CurrentChar(), chars))
		{
			if (CurrentChar() == '"' || CurrentChar() == '\'')
			{
				// this is the case that match is inside a string!
				char ch = CurrentChar();
				MoveToNextChar();
				SkipToChar(ch);
			}
			MoveToNextChar();
			if (supportNesting)
			{
				if (CurrentChar() == '{')
					SkipBlock('{');
			}
		}
		if (PreviousChar() != '\\')
			break;
		else
		{
			// check for "\\"
			if (m_TokenIndex - 2 >= 0 && m_Buffer.GetChar(m_TokenIndex - 2) == '\\')
				break;
		}
		MoveToNextChar();
	}
	if (IsEOF())
		return false;
	return true;
}

bool Tokenizer::SkipToEOL()
{
	// skip everything until we find EOL
	while (1)
	{
		while (!IsEOF() && CurrentChar() != '\n')
			MoveToNextChar();
		wxChar last = PreviousChar();
		// if DOS line endings, we 've hit \r and we skip to \n...
		if (last == '\r')
			last = m_Buffer.GetChar(m_TokenIndex - 2);
		if (IsEOF() || last != '\\')
			break;
		else
			MoveToNextChar();
	}
	if (IsEOF())
		return false;
	return true;
}

bool Tokenizer::SkipBlock(const wxChar& ch)
{
	// skip blocks () [] {} <>
	wxChar match;
	switch (ch)
	{
		case '(': match = ')'; break;
		case '[': match = ']'; break;
		case '{': match = '}'; break;
		case '<': match = '>'; break;
		default : return false;
	}
	
	MoveToNextChar();
	int count = 1; // counter for nested blocks (xxx())
	while (!IsEOF())
	{
		if (CurrentChar() == '"' || CurrentChar() == '\'')
		{
			// this is the case that match is inside a string!
			char ch = CurrentChar();
			MoveToNextChar();
			SkipToChar(ch);
			MoveToNextChar();
		}
		if (CurrentChar() == ch)
			++count;
		else if (CurrentChar() == match)
			--count;
		MoveToNextChar();
		if (count == 0)
			break;
	}
	if (IsEOF())
		return false;
	return true;
}

bool Tokenizer::SkipUnwanted()
{
	while (CurrentChar() == '#' ||
			(!m_IsOperator && CurrentChar() == '=') ||
			(!m_IsOperator && CurrentChar() == '[') ||
			CurrentChar() == '?' ||
			m_Buffer.Mid(m_TokenIndex, 2) == "//" ||
			m_Buffer.Mid(m_TokenIndex, 2) == "/*")
	{
		bool skipPreprocessor = false; // used for #include
		while (m_Buffer.Mid(m_TokenIndex, 2) == "//" ||
				m_Buffer.Mid(m_TokenIndex, 2) == "/*")
		{
			// C/C++ style comments
			bool cstyle = NextChar() == '*';
			MoveToNextChar();
			MoveToNextChar();
			while (1)
			{
				if (!cstyle)
				{
					if (!SkipToEOL())
						return false;
					MoveToNextChar();
					break;
				}
				else
				{
					if (SkipToChar('/'))
					{
						if (PreviousChar() == '*')
						{
							MoveToNextChar();
							break;
						}
						MoveToNextChar();
					}
					else
						return false;
				}
			}
			if (IsEOF())
				return false;
			if (!SkipWhiteSpace())
				return false;
		}
		
		while (CurrentChar() == '#')
		{
			// preprocessor directives
			// we only care for #include and #define, for now
			unsigned int backupIdx = m_TokenIndex;
			MoveToNextChar();
			SkipWhiteSpace();
			if ((CurrentChar() == 'i' && NextChar() == 'n') || // in(clude)
				(m_Options.wantPreprocessor && CurrentChar() == 'd' && NextChar() == 'e')) // de(fine)
			{
				// ok, we have something like #in(clude)
				m_LastWasPreprocessor = true;
				m_LastPreprocessor.Clear();
				m_TokenIndex = backupIdx; // keep #
				skipPreprocessor = true;
				break;
			}
			else
			{
				// skip the rest for now...
				SkipToEOL();
				if (!SkipWhiteSpace())
					return false;
			}
			if (skipPreprocessor)
				break;
		}

		while (CurrentChar() == '[')
		{
			// array subscripts
			// skip them for now...
			SkipBlock('[');
			if (!SkipWhiteSpace())
				return false;
		}
	
		while (CurrentChar() == '=')
		{
			// skip assignments
			// TODO: what happens with operators?
			if (!SkipToOneOfChars(",;}", true))
				return false;
		}
	
		while (CurrentChar() == '?')
		{
			// skip "condition ? true : false"
			// TODO: what happens with operators?
			if (!SkipToOneOfChars(";}"))
				return false;
		}
		if (skipPreprocessor)
			break;
	}
	return true;
}

wxString Tokenizer::GetToken()
{
	m_UndoTokenIndex = m_TokenIndex;
	m_UndoLineNumber = m_LineNumber;
	m_UndoNestLevel = m_NestLevel;
	return DoGetToken();
}

wxString Tokenizer::PeekToken()
{
	unsigned int undoTokenIndex = m_TokenIndex;
	unsigned int undoLineNumber = m_LineNumber;
	unsigned int undoNestLevel = m_NestLevel;
	wxString peek = DoGetToken();
	m_TokenIndex = undoTokenIndex;
	m_LineNumber = undoLineNumber;
	m_NestLevel = undoNestLevel;
	return peek;
}

void Tokenizer::UngetToken()
{
	m_TokenIndex = m_UndoTokenIndex;
	m_LineNumber = m_UndoLineNumber;
	m_NestLevel = m_UndoNestLevel;
}

wxString Tokenizer::DoGetToken()
{
	if (IsEOF())
		return wxEmptyString;
	
	if (!SkipWhiteSpace())
		return wxEmptyString;

	if (!SkipUnwanted())
		return wxEmptyString;
	
	int start = m_TokenIndex;
	wxString m_Str;

	if (isalpha(CurrentChar()) || CurrentChar() == '_')
	{
		// keywords, identifiers, etc.
		while (!IsEOF() &&
				(isalnum(CurrentChar()) ||
				CurrentChar() == '_'))
			MoveToNextChar();
		if (IsEOF())
			return wxEmptyString;
		m_Str = m_Buffer.Mid(start, m_TokenIndex - start);
		m_IsOperator = m_Str.Matches("operator");
	}
	else if (isdigit(CurrentChar()))
	{
		// numbers
		while (!IsEOF() && CharInString(CurrentChar(), "0123456789.abcdefABCDEFfXxLl"))
			MoveToNextChar();
		if (IsEOF())
			return wxEmptyString;
		m_Str = m_Buffer.Mid(start, m_TokenIndex - start);
		m_IsOperator = false;
	}
	else if (CurrentChar() == '"' ||
			CurrentChar() == '\'')
	{
		// string, char, etc.
		wxChar match = CurrentChar();
		MoveToNextChar();  // skip starting ' or "
		if (!SkipToChar(match))
			return wxEmptyString;
		MoveToNextChar(); // skip ending ' or "
		m_Str = m_Buffer.Mid(start, m_TokenIndex - start);
	}
	else if (CurrentChar() == ':')
	{
		if (NextChar() == ':')
		{
			MoveToNextChar();
			MoveToNextChar();
			m_Str = "::";
		}
		else
		{
			MoveToNextChar();
			m_Str = ":";
		}
	}
	else if (CurrentChar() == '(')
	{
		m_IsOperator = false;
		// skip blocks () []
		if (!SkipBlock(CurrentChar()))
			return wxEmptyString;
		wxString tmp = m_Buffer.Mid(start, m_TokenIndex - start);
		tmp.Replace("\t", " "); // replace tabs with spaces
		tmp.Replace("\n", " "); // replace LF with spaces
		tmp.Replace("\r", " "); // replace CR with spaces
		// fix-up arguments (remove excessive spaces/tabs/newlines)
		for (unsigned int i = 0; i < tmp.Length() - 1; ++i)
		{
			if (tmp.GetChar(i) == '/' && tmp.GetChar(i + 1) == '*')
			{
				// skip C comments
				i += 2;
				while (i < tmp.Length() - 1)
				{
					if (tmp.GetChar(i) == '*' && tmp.GetChar(i + 1) == '/')
						break;
					++i;
				}
				if (i >= tmp.Length() - 1 || tmp.GetChar(i + 1) != '/')
					continue; // we failed...
				i += 2;
			}
			else if (tmp.GetChar(i) == '=')
			{
				// skip default assignments
				++i;
				int level = 0; // nesting parenthesis
				while (i < tmp.Length())
				{
					if (tmp.GetChar(i) == '(')
						++level;
					else if (tmp.GetChar(i) == ')')
						--level;
					if ((tmp.GetChar(i) == ',' && level == 0) || 
						(tmp.GetChar(i) == ')' && level < 0))
						break;
					++i;
				}
                if (i < tmp.Length() && tmp.GetChar(i) == ',')
                    --i;
				continue; // we are done here
			}
			
			if (i < tmp.Length() - 1 && tmp.GetChar(i) == ' ' && tmp.GetChar(i + 1) == ' ')
				continue; // skip excessive spaces
			m_Str << tmp.GetChar(i);
		}
		m_Str << ')'; // add closing parenthesis (see "i < tmp.Length() - 1" in previous "for")
		m_Str.Replace("  ", " "); // replace two-spaces with single-space (introduced if it skipped comments or assignments)
		m_Str.Replace("( ", "(");
		m_Str.Replace(" )", ")");
	}
	else
	{
		if (CurrentChar() == '{')
			++m_NestLevel;
		else if (CurrentChar() == '}')
			--m_NestLevel;
		m_Str = CurrentChar();
		MoveToNextChar();
	}

	if (m_LastWasPreprocessor && !m_Str.Matches("#") && !m_LastPreprocessor.Matches("#"))
	{
		if (!m_LastPreprocessor.Matches("#include"))
		{
			// except for #include, all other preprocessor directives need only
			// one word exactly after the directive, e.g. #define THIS_WORD
			SkipToEOL();
		}
		m_LastWasPreprocessor = false;
		m_LastPreprocessor.Clear();
	}

	if (m_LastWasPreprocessor)
		m_LastPreprocessor << m_Str;
	
	return m_Str;
}
