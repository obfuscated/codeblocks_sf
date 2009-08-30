/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   Copyright (C) 2006-2009 by Jim Pattee <jimp03@email.com>
 *   Copyright (C) 1998-2002 by Tal Davidson
 *   <http://www.gnu.org/licenses/lgpl-3.0.html>
 *
 *   This file is a part of Artistic Style - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   <http://astyle.sourceforge.net>
 *
 *   Artistic Style is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Artistic Style is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Artistic Style.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "astyle.h"


namespace astyle
{

// ---------------------------- functions for ASEnhancer Class -------------------------------------

/**
 * ASEnhancer constructor
 */
ASEnhancer::ASEnhancer()
{
	// the following prevents warning messages with cppcheck
	// it will NOT compile if activated
//	init();
}

/**
 * Destructor of ASEnhancer
 */
ASEnhancer::~ASEnhancer()
{
}

/**
 * initialize the ASEnhancer.
 *
 * init() is called each time an ASFormatter object is initialized.
 */
void ASEnhancer::init(int fileType,
                      int _indentLength,
                      string _indentString,
                      bool _caseIndent,
                      bool _emptyLineFill)
{
	// formatting variables from ASFormatter and ASBeautifier
	ASBase::init(fileType);
	indentLength = _indentLength;
	if (_indentString == "\t")
		useTabs = true;
	else
		useTabs = false;

	caseIndent    = _caseIndent;
	emptyLineFill = _emptyLineFill;
	quoteChar = '\'';

	// unindent variables
	lineNumber = 0;
	bracketCount = 0;
	isInComment = false;
	isInQuote = false;
	switchDepth = 0;
	lookingForCaseBracket = false;
	unindentNextLine = false;

	// switch struct and vector
	sw.switchBracketCount = 0;
	sw.unindentDepth = 0;
	sw.unindentCase = false;
	swVector.clear();

	nextLineIsEventTable = false;
	isInEventTable = false;
}

/**
 * additional formatting for line of source code.
 * every line of source code in a source code file should be sent
 *     one after the other to this function.
 * indents event tables
 * unindents the case blocks
 *
 * @param line       the original formatted line will be updated if necessary.
 */
void ASEnhancer::enhance(string &line)
{
	bool   isSpecialChar = false;
	size_t  lineLength = line.length();

	lineNumber++;

	// check for beginning of event table
	if (nextLineIsEventTable)
	{
		isInEventTable = true;
		nextLineIsEventTable = false;
	}

	if (lineLength == 0
	        && ! isInEventTable
	        && ! emptyLineFill)
		return;

	// test for unindent on attached brackets
	if (unindentNextLine)
	{
		sw.unindentDepth++;
		sw.unindentCase = true;
		unindentNextLine = false;
//      cout << " unindent case " << sw.unindentDepth << endl;
	}

	// parse characters in the current line.

	for (size_t i = 0; i < lineLength; i++)
	{
		char ch = line[i];

		// bypass whitespace
		if (isWhiteSpace(ch))
			continue;

		// handle special characters (i.e. backslash+character such as \n, \t, ...)
		if (isSpecialChar)
		{
			isSpecialChar = false;
			continue;
		}
		if (!(isInComment) && line.compare(i, 2, "\\\\") == 0)
		{
			i++;
			continue;
		}
		if (!(isInComment) && ch == '\\')
		{
			isSpecialChar = true;
			continue;
		}

		// handle quotes (such as 'x' and "Hello Dolly")
		if (!(isInComment) && (ch == '"' || ch == '\''))
		{
			if (!isInQuote)
			{
				quoteChar = ch;
				isInQuote = true;
			}
			else if (quoteChar == ch)
			{
				isInQuote = false;
				continue;
			}
		}

		if (isInQuote)
			continue;

		// handle comments

		if (!(isInComment) && line.compare(i, 2, "//") == 0)
		{
			// check for windows line markers
			if (line.compare(i + 2, 1, "\xf0") > 0)
				lineNumber--;
			break;                 // finished with the line
		}
		else if (!(isInComment) && line.compare(i, 2, "/*") == 0)
		{
			isInComment = true;
			i++;
			continue;
		}
		else if ((isInComment) && line.compare(i, 2, "*/") == 0)
		{
			isInComment = false;
			i++;
			continue;
		}

		if (isInComment)
			continue;

		// if we have reached this far then we are NOT in a comment or string of special characters

		if (line[i] == '{')
			bracketCount++;

		if (line[i] == '}')
			bracketCount--;

		bool isPotentialKeyword = isCharPotentialHeader(line, i);

		// ----------------  process event tables  --------------------------------------

		// check for event table begin
		if (isPotentialKeyword)
		{
			if (findKeyword(line, i, "BEGIN_EVENT_TABLE")
			        || findKeyword(line, i, "BEGIN_MESSAGE_MAP"))
				nextLineIsEventTable = true;

			// check for event table end
			if (findKeyword(line, i, "END_EVENT_TABLE")
			        || findKeyword(line, i, "END_MESSAGE_MAP"))
				isInEventTable = false;
		}

		// ----------------  process switch statements  ---------------------------------

		if (isPotentialKeyword && findKeyword(line, i, "switch"))
		{
			switchDepth++;
//          cout << " switch " <<  switchDepth << endl;
			swVector.push_back(sw);                         // save current variables
			sw.switchBracketCount = 0;
			sw.unindentCase = false;                        // don't clear case until end of switch
			i += 5;                                         // bypass switch statement
			continue;
		}

		// just want switch statements from this point

		if (caseIndent || switchDepth == 0)               // from here just want switch statements
			continue;

		if (line[i] == '{')
		{
			sw.switchBracketCount++;
			if (lookingForCaseBracket)                      // if 1st after case statement
			{
				sw.unindentCase = true;                     // unindenting this case
				sw.unindentDepth++;
				lookingForCaseBracket = false;              // not looking now
//              cout << " unindent case " <<  sw.unindentDepth << endl;
			}
			continue;
		}

		lookingForCaseBracket = false;                      // no opening bracket, don't indent

		if (line[i] == '}')                                 // if close bracket
		{
			sw.switchBracketCount--;
			if (sw.switchBracketCount == 0)                 // if end of switch statement
			{
//              cout << "  endsw " << switchDepth << endl;
				switchDepth--;                              // one less switch
				sw = swVector.back();                       // restore sw struct
				swVector.pop_back();                        // remove last entry from stack
			}
			continue;
		}

		// look for case or default header

		if (isPotentialKeyword
		        && (findKeyword(line, i, "case") || findKeyword(line, i, "default")))
		{
			if (sw.unindentCase)                            // if unindented last case
			{
				sw.unindentCase = false;                    // stop unindenting previous case
				sw.unindentDepth--;                         // reduce depth
			}
			bool isInQuote = false;
			char quoteChar = ' ';
			for (; i < lineLength; i++)                     // find colon
			{
				if (isInQuote)
				{
					if (line[i] == '\\')
					{
						i++;                                // bypass next char
						continue;
					}
					else if (line[i] == quoteChar)          // check ending quote
					{
						isInQuote = false;
						quoteChar = ' ';
						continue;
					}
					else
					{
						continue;                           // must close quote before continuing
					}
				}
				if (line[i] == '\'' || line[i] == '\"')		// check opening quote
				{
					isInQuote = true;
					quoteChar = line[i];
					continue;
				}
				if (line[i] == ':')
				{
					if ((i + 1 < lineLength) && (line[i + 1] == ':'))
						i++;                                // bypass scope resolution operator
					else
						break;                              // found it
				}
			}
			i++;
			for (; i < lineLength; i++)                     // bypass whitespace
			{
				if (!(isWhiteSpace(line[i])))
					break;
			}
			if (i < lineLength)
			{
				if (line[i] == '{')
				{
					sw.switchBracketCount++;
					unindentNextLine = true;
					continue;
				}
			}
			lookingForCaseBracket = true;                   // bracket must be on next line
			i--;                                            // need to check for comments
			continue;
		}

		if (isPotentialKeyword)
		{
			string name = getCurrentWord(line, i);          // bypass the entire name
			i += name.length() - 1;
		}

	}   // end of for loop

	if (isInEventTable)                                     // if need to indent
	{
		if (line[0] != '#')
			indentLine(line, 1);
	}

	if (sw.unindentDepth > 0)                               // if need to unindent
		unindentLine(line, sw.unindentDepth);
}

/**
 * indent a line by a given number of tabsets
 *    by inserting leading whitespace to the line argument.
 *
 * @param line          a pointer to the line to indent.
 * @param unindent      the number of tabsets to insert.
 * @return              the number of characters inserted.
 */
int ASEnhancer::indentLine(string  &line, const int indent) const
{
	if (line.length() == 0
	        && ! emptyLineFill)
		return 0;

	size_t charsToInsert;                       // number of chars to insert

	if (useTabs)                                // if formatted with tabs
	{
		charsToInsert = indent;                 // tabs to insert
		line.insert((size_t) 0, charsToInsert, '\t');    // insert the tabs
	}
	else
	{
		charsToInsert = indent * indentLength;  // compute chars to insert
		line.insert((size_t)0, charsToInsert, ' ');     // insert the spaces
	}

	return charsToInsert;
}

/**
 * unindent a line by a given number of tabsets
 *    by erasing the leading whitespace from the line argument.
 *
 * @param line          a pointer to the line to unindent.
 * @param unindent      the number of tabsets to erase.
 * @return              the number of characters erased.
 */
int ASEnhancer::unindentLine(string  &line, const int unindent) const
{
	size_t whitespace = line.find_first_not_of(" \t");

	if (whitespace == string::npos)         // if line is blank
		whitespace = line.length();         // must remove padding, if any

	if (whitespace == 0)
		return 0;

	size_t charsToErase;                    // number of chars to erase

	if (useTabs)                            // if formatted with tabs
	{
		charsToErase = unindent;            // tabs to erase
		if (charsToErase <= whitespace)     // if there is enough whitespace
			line.erase(0, charsToErase);    // erase the tabs
		else
			charsToErase = 0;
	}
	else
	{
		charsToErase = unindent * indentLength; // compute chars to erase
		if (charsToErase <= whitespace)         // if there is enough whitespace
			line.erase(0, charsToErase);        // erase the spaces
		else
			charsToErase = 0;
	}

	return charsToErase;
}


}   // end namespace astyle
