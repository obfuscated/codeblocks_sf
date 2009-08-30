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

#include "astyle_main.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <errno.h>

// includes for recursive getFileNames() function
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#ifdef __VMS
#include <unixlib.h>
#include <rms.h>
#include <ssdef.h>
#include <stsdef.h>
#include <lib$routines.h>
#include <starlet.h>
#endif /* __VMS */
#endif

// turn off MinGW automatic file globbing
// this CANNOT be in the astyle namespace
int _CRT_glob = 0;

namespace astyle
{

#define IS_OPTION(arg,op)          ((arg).compare(op)==0)
#define IS_OPTIONS(arg,a,b)        (IS_OPTION((arg),(a)) || IS_OPTION((arg),(b)))

#define GET_PARAM(arg,op)          ((arg).substr(strlen(op)))
#define GET_PARAMS(arg,a,b) (isParamOption((arg),(a)) ? GET_PARAM((arg),(a)) : GET_PARAM((arg),(b)))

#ifdef _WIN32
char g_fileSeparator = '\\';
bool g_isCaseSensitive = false;
#else
char g_fileSeparator = '/';
bool g_isCaseSensitive = true;
#endif

#ifdef ASTYLE_LIB
// library build variables
stringstream* _err = NULL;
#else
// console build variables
ostream* _err = &cerr;           // direct error messages to cerr
ASConsole* g_console = NULL;     // class to encapsulate console variables
#endif

#ifdef ASTYLE_JNI
// java library build variables
JNIEnv*   g_env;
jobject   g_obj;
jmethodID g_mid;
#endif

const char* _version = "1.24 beta";


/**
 * parse the options vector
 * ITER can be either a fileOptionsVector (options file) or an optionsVector (command line)
 *
 * @return        true if no errors, false if errors
 */
template<typename ITER>
bool parseOptions(ASFormatter &formatter,
                  const ITER &optionsBegin,
                  const ITER &optionsEnd,
                  const string &errorInfo)
{
	ITER option;
	bool ok = true;
	string arg, subArg;

	for (option = optionsBegin; option != optionsEnd; ++option)
	{
		arg = *option;

		if (arg.compare(0, 2, "--") == 0)
			ok &= parseOption(formatter, arg.substr(2), errorInfo);
		else if (arg[0] == '-')
		{
			size_t i;

			for (i = 1; i < arg.length(); ++i)
			{
				if (isalpha(arg[i]) && i > 1)
				{
					ok &= parseOption(formatter, subArg, errorInfo);
					subArg = "";
				}
				subArg.append(1, arg[i]);
			}
			ok &= parseOption(formatter, subArg, errorInfo);
			subArg = "";
		}
		else
		{
			ok &= parseOption(formatter, arg, errorInfo);
			subArg = "";
		}
	}
	return ok;
}

void importOptions(istream &in, vector<string> &optionsVector)
{
	char ch;
	string currentToken;

	while (in)
	{
		currentToken = "";
		do
		{
			in.get(ch);
			if (in.eof())
				break;
			// treat '#' as line comments
			if (ch == '#')
				while (in)
				{
					in.get(ch);
					if (ch == '\n')
						break;
				}

			// break options on spaces, tabs, commas, or new-lines
			if (in.eof() || ch == ' ' || ch == '\t' || ch == ',' || ch == '\n')
				break;
			else
				currentToken.append(1, ch);

		}
		while (in);

		if (currentToken.length() != 0)
			optionsVector.push_back(currentToken);
	}
}

void isOptionError(const string &arg, const string &errorInfo)
{
#ifdef ASTYLE_LIB
	if (_err->str().length() == 0)
	{
		(*_err) << errorInfo << endl;   // need main error message
		(*_err) << arg;                 // output the option in error
	}
	else
		(*_err) << endl << arg;         // put endl after previous option
#else
	if (errorInfo.length() > 0)         // to avoid a compiler warning
		g_console->error("Error in param: ", arg.c_str());
#endif
}


bool isParamOption(const string &arg, const char *option)
{
	bool retVal = arg.compare(0, strlen(option), option) == 0;
	// if comparing for short option, 2nd char of arg must be numeric
	if (retVal && strlen(option) == 1 && arg.length() > 1)
		if (!isdigit(arg[1]))
			retVal = false;
	return retVal;
}

bool isParamOption(const string &arg, const char *option1, const char *option2)
{
	return isParamOption(arg, option1) || isParamOption(arg, option2);
}

bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo)
{
	if ( IS_OPTION(arg, "style=allman") || IS_OPTION(arg, "style=ansi")  || IS_OPTION(arg, "style=bsd") )
	{
		formatter.setFormattingStyle(STYLE_ALLMAN);
	}
	else if ( IS_OPTION(arg, "style=java") )
	{
		formatter.setFormattingStyle(STYLE_JAVA);
	}
	else if ( IS_OPTION(arg, "style=k&r") || IS_OPTION(arg, "style=k/r") )
	{
		formatter.setFormattingStyle(STYLE_KandR);
	}
	else if ( IS_OPTION(arg, "style=stroustrup") )
	{
		formatter.setFormattingStyle(STYLE_STROUSTRUP);
	}
	else if ( IS_OPTION(arg, "style=whitesmith") )
	{
		formatter.setFormattingStyle(STYLE_WHITESMITH);
	}
	else if ( IS_OPTION(arg, "style=banner") )
	{
		formatter.setFormattingStyle(STYLE_BANNER);
	}
	else if ( IS_OPTION(arg, "style=gnu") )
	{
		formatter.setFormattingStyle(STYLE_GNU);
	}
	else if ( IS_OPTION(arg, "style=linux") )
	{
		formatter.setFormattingStyle(STYLE_LINUX);
	}
	else if ( IS_OPTION(arg, "style=horstmann") )
	{
		formatter.setFormattingStyle(STYLE_HORSTMANN);
	}
	else if ( IS_OPTION(arg, "style=1tbs") || IS_OPTION(arg, "style=otbs") )
	{
		formatter.setFormattingStyle(STYLE_1TBS);
	}
	else if ( isParamOption(arg, "A") )
	{
		int style = 0;
		string styleParam = GET_PARAM(arg, "A");
		if (styleParam.length() > 0)
			style = atoi(styleParam.c_str());
		if (style < 1 || style > 10)
			isOptionError(arg, errorInfo);
		else if (style == 1)
			formatter.setFormattingStyle(STYLE_ALLMAN);
		else if (style == 2)
			formatter.setFormattingStyle(STYLE_JAVA);
		else if (style == 3)
			formatter.setFormattingStyle(STYLE_KandR);
		else if (style == 4)
			formatter.setFormattingStyle(STYLE_STROUSTRUP);
		else if (style == 5)
			formatter.setFormattingStyle(STYLE_WHITESMITH);
		else if (style == 6)
			formatter.setFormattingStyle(STYLE_BANNER);
		else if (style == 7)
			formatter.setFormattingStyle(STYLE_GNU);
		else if (style == 8)
			formatter.setFormattingStyle(STYLE_LINUX);
		else if (style == 9)
			formatter.setFormattingStyle(STYLE_HORSTMANN);
		else if (style == 10)
			formatter.setFormattingStyle(STYLE_1TBS);
	}
	// must check for mode=cs before mode=c !!!
	else if ( IS_OPTION(arg, "mode=cs") )
	{
		formatter.setSharpStyle();
		formatter.setModeManuallySet(true);
	}
	else if ( IS_OPTION(arg, "mode=c") )
	{
		formatter.setCStyle();
		formatter.setModeManuallySet(true);
	}
	else if ( IS_OPTION(arg, "mode=java") )
	{
		formatter.setJavaStyle();
		formatter.setModeManuallySet(true);
	}
	else if ( isParamOption(arg, "t", "indent=tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "t", "indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setTabIndentation(spaceNum, false);
			formatter.setIndentManuallySet(true);
		}
	}
	else if ( IS_OPTION(arg, "indent=tab") )
	{
		formatter.setTabIndentation(4);
	}
	else if ( isParamOption(arg, "T", "indent=force-tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "indent=force-tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setTabIndentation(spaceNum, true);
	}
	else if ( IS_OPTION(arg, "indent=force-tab") )
	{
		formatter.setTabIndentation(4, true);
	}
	else if ( isParamOption(arg, "s", "indent=spaces=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "s", "indent=spaces=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setSpaceIndentation(spaceNum);
			formatter.setIndentManuallySet(true);
		}
	}
	else if ( IS_OPTION(arg, "indent=spaces") )
	{
		formatter.setSpaceIndentation(4);
	}
	else if ( isParamOption(arg, "m", "min-conditional-indent=") )
	{
		int minIndent = 8;
		string minIndentParam = GET_PARAMS(arg, "m", "min-conditional-indent=");
		if (minIndentParam.length() > 0)
			minIndent = atoi(minIndentParam.c_str());
		if (minIndent > 40)
			isOptionError(arg, errorInfo);
		else
			formatter.setMinConditionalIndentLength(minIndent);
	}
	else if ( isParamOption(arg, "M", "max-instatement-indent=") )
	{
		int maxIndent = 40;
		string maxIndentParam = GET_PARAMS(arg, "M", "max-instatement-indent=");
		if (maxIndentParam.length() > 0)
			maxIndent = atoi(maxIndentParam.c_str());
		if (maxIndent > 80)
			isOptionError(arg, errorInfo);
		else
			formatter.setMaxInStatementIndentLength(maxIndent);
	}
	else if ( IS_OPTIONS(arg, "B", "indent-brackets") )
	{
		formatter.setBracketIndent(true);
	}
	else if ( IS_OPTIONS(arg, "G", "indent-blocks") )
	{
		formatter.setBlockIndent(true);
	}
	else if ( IS_OPTIONS(arg, "N", "indent-namespaces") )
	{
		formatter.setNamespaceIndent(true);
	}
	else if ( IS_OPTIONS(arg, "C", "indent-classes") )
	{
		formatter.setClassIndent(true);
	}
	else if ( IS_OPTIONS(arg, "S", "indent-switches") )
	{
		formatter.setSwitchIndent(true);
	}
	else if ( IS_OPTIONS(arg, "K", "indent-cases") )
	{
		formatter.setCaseIndent(true);
	}
	else if ( IS_OPTIONS(arg, "L", "indent-labels") )
	{
		formatter.setLabelIndent(true);
	}
	else if ( IS_OPTIONS(arg, "y", "break-closing-brackets") )
	{
		formatter.setBreakClosingHeaderBracketsMode(true);
	}
	else if ( IS_OPTIONS(arg, "b", "brackets=break") )
	{
		formatter.setBracketFormatMode(BREAK_MODE);
	}
	else if ( IS_OPTIONS(arg, "a", "brackets=attach") )
	{
		formatter.setBracketFormatMode(ATTACH_MODE);
	}
	else if ( IS_OPTIONS(arg, "l", "brackets=linux") )
	{
		formatter.setBracketFormatMode(LINUX_MODE);
	}
	else if ( IS_OPTIONS(arg, "u", "brackets=stroustrup") )
	{
		formatter.setBracketFormatMode(STROUSTRUP_MODE);
	}
	else if ( IS_OPTIONS(arg, "g", "brackets=horstmann") )
	{
		formatter.setBracketFormatMode(HORSTMANN_MODE);
	}
	else if ( IS_OPTIONS(arg, "O", "keep-one-line-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTIONS(arg, "o", "keep-one-line-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTIONS(arg, "P", "pad-paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "d", "pad-paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "D", "pad-paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "H", "pad-header") )
	{
		formatter.setParensHeaderPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "U", "unpad-paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "p", "pad-oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "x", "delete-empty-lines") )
	{
		formatter.setDeleteEmptyLinesMode(true);
	}
	else if ( IS_OPTIONS(arg, "E", "fill-empty-lines") )
	{
		formatter.setEmptyLineFill(true);
	}
	else if ( IS_OPTIONS(arg, "w", "indent-preprocessor") )
	{
		formatter.setPreprocessorIndent(true);
	}
	else if ( IS_OPTIONS(arg, "c", "convert-tabs") )
	{
		formatter.setTabSpaceConversionMode(true);
	}
	else if ( IS_OPTIONS(arg, "F", "break-blocks=all") )
	{
		formatter.setBreakBlocksMode(true);
		formatter.setBreakClosingHeaderBlocksMode(true);
	}
	else if ( IS_OPTIONS(arg, "f", "break-blocks") )
	{
		formatter.setBreakBlocksMode(true);
	}
	else if ( IS_OPTIONS(arg, "e", "break-elseifs") )
	{
		formatter.setBreakElseIfsMode(true);
	}
	else if ( IS_OPTIONS(arg, "{", "add-brackets") )
	{
		formatter.setAddBracketsMode(true);
	}
	else if ( IS_OPTIONS(arg, "!", "add-one-line-brackets") )
	{
		formatter.setAddOneLineBracketsMode(true);
	}
	else if ( IS_OPTION(arg, "align-pointer=type") )
	{
		formatter.setPointerAlignment(ALIGN_TYPE);
	}
	else if ( IS_OPTION(arg, "align-pointer=middle") )
	{
		formatter.setPointerAlignment(ALIGN_MIDDLE);
	}
	else if ( IS_OPTION(arg, "align-pointer=name") )
	{
		formatter.setPointerAlignment(ALIGN_NAME);
	}
	else if ( isParamOption(arg, "k") )
	{
		int align = 0;
		string styleParam = GET_PARAM(arg, "k");
		if (styleParam.length() > 0)
			align = atoi(styleParam.c_str());
		if (align < 1 || align > 3)
			isOptionError(arg, errorInfo);
		else if (align == 1)
			formatter.setPointerAlignment(ALIGN_TYPE);
		else if (align == 2)
			formatter.setPointerAlignment(ALIGN_MIDDLE);
		else if (align == 3)
			formatter.setPointerAlignment(ALIGN_NAME);
	}
	// depreciated options /////////////////////////////////////////////////////////////////////////////////////
	// depreciated in release 1.22 - may be removed at an appropriate time
	else if ( IS_OPTION(arg, "style=kr") )
	{
		formatter.setFormattingStyle(STYLE_JAVA);
	}
	else if ( isParamOption(arg, "T", "force-indent=tab=") )
	{
		// the 'T' option will already have been processed
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "force-indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 1 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
			formatter.setTabIndentation(spaceNum, true);
	}
	else if ( IS_OPTION(arg, "brackets=break-closing") )
	{
		formatter.setBreakClosingHeaderBracketsMode(true);
	}

	else if ( IS_OPTION(arg, "one-line=keep-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTION(arg, "one-line=keep-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTION(arg, "pad=paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTION(arg, "unpad=paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTION(arg, "pad=oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	// end depreciated options //////////////////////////////////////////////////////////////////////////////
#ifdef ASTYLE_LIB
	// End of options used by GUI
	else
		isOptionError(arg, errorInfo);
#else
	// Options used by only console
	else if ( IS_OPTIONS(arg, "n", "suffix=none") )
	{
		g_console->noBackup = true;
	}
	else if ( isParamOption(arg, "suffix=") )
	{
		string suffixParam = GET_PARAM(arg, "suffix=");
		if (suffixParam.length() > 0)
		{
			g_console->origSuffix = suffixParam;
		}
	}
	else if ( isParamOption(arg, "exclude=") )
	{
		string suffixParam = GET_PARAM(arg, "exclude=");
		if (suffixParam.length() > 0)
		{
			g_console->standardizePath(suffixParam, true);
			g_console->excludeVector.push_back(suffixParam);
			g_console->excludeHitsVector.push_back(false);
		}
	}
	else if ( IS_OPTIONS(arg, "r", "R") || IS_OPTION(arg, "recursive") )
	{
		g_console->isRecursive = true;
	}
	else if ( IS_OPTIONS(arg, "v", "verbose") )
	{
		g_console->isVerbose = true;
	}
	else if ( IS_OPTIONS(arg, "Q", "formatted") )
	{
		g_console->isFormattedOnly = true;
	}
	else if ( IS_OPTIONS(arg, "q", "quiet") )
	{
		g_console->isQuiet = true;
	}
	else if ( IS_OPTIONS(arg, "X", "errors-to-stdout") )
	{
		_err = &cout;
	}
	else
	{
		(*_err) << errorInfo << arg << endl;
		return false; // invalid option
	}
#endif
// End of parseOption function
	return true; //o.k.
}

//--------------------------------------------------------------------------------------
// ASStreamIterator class
// typename will be istringstream for GUI and istream otherwise
//--------------------------------------------------------------------------------------

template<typename T>
ASStreamIterator<T>::ASStreamIterator(T *in)
{
	inStream = in;
	buffer.reserve(200);
	eolWindows = eolLinux = eolMacOld = 0;
	peekStart = 0;
	prevLineDeleted = false;
	checkForEmptyLine = false;
}

template<typename T>
ASStreamIterator<T>::~ASStreamIterator()
{
}

// save the last input line after input has reached EOF
template<typename T>
void ASStreamIterator<T>::saveLastInputLine()
{
	assert(inStream->eof());
	prevBuffer = buffer;
}

/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */
template<typename T>
string ASStreamIterator<T>::nextLine(bool emptyLineWasDeleted)
{
	// verify that the current position is correct
	assert (peekStart == 0);

	// a deleted line may be replaced if break-blocks is requested
	// this sets up the compare to check for a replaced empty line
	if (prevLineDeleted)
	{
		prevLineDeleted = false;
		checkForEmptyLine = true;
	}
	if (!emptyLineWasDeleted)
		prevBuffer = buffer;
	else
		prevLineDeleted = true;

	// read the next record
	buffer.clear();
	char ch;
	inStream->get(ch);

	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		buffer.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return buffer;
	}

	int peekCh = inStream->peek();

	// find input end-of-line characters
	if (!inStream->eof())
	{
		if (ch == '\r')         // CR+LF is windows otherwise Mac OS 9
		{
			if (peekCh == '\n')
			{
				inStream->get(ch);
				eolWindows++;
			}
			else
				eolMacOld++;
		}
		else                    // LF is Linux, allow for improbable LF/CR
		{
			if (peekCh == '\r')
			{
				inStream->get(ch);
				eolWindows++;
			}
			else
				eolLinux++;
		}
	}
	else
	{
		inStream->clear();
	}

	// set output end of line characters
	if (eolWindows >= eolLinux)
	{
		if (eolWindows >= eolMacOld)
			strcpy(outputEOL, "\r\n");  // Windows (CR+LF)
		else
			strcpy(outputEOL, "\r");    // MacOld (CR)
	}
	else if (eolLinux >= eolMacOld)
		strcpy(outputEOL, "\n");    // Linux (LF)
	else
		strcpy(outputEOL, "\r");    // MacOld (CR)

	return buffer;
}

// save the current position and get the next line
// this can be called for multiple reads
// when finished peeking you MUST call peekReset()
// call this function from ASFormatter ONLY
template<typename T>
string ASStreamIterator<T>::peekNextLine()
{
	assert (hasMoreLines());
	string nextLine;
	char ch;

	if (peekStart == 0)
		peekStart = inStream->tellg();

	// read the next record
	inStream->get(ch);
	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		nextLine.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return nextLine;
	}

	int peekCh = inStream->peek();

	// remove end-of-line characters
	if (!inStream->eof())
	{
		if ((peekCh == '\n' || peekCh == '\r') && peekCh != ch)  /////////////  changed  //////////
			inStream->get(ch);
	}

	return nextLine;
}

// reset current position and EOF for peekNextLine()
template<typename T>
void ASStreamIterator<T>::peekReset()
{
	assert(peekStart != 0);
	inStream->clear();
	inStream->seekg(peekStart);
	peekStart = 0;
}

#ifndef ASTYLE_LIB
//--------------------------------------------------------------------------------------
// ASConsole class
// main function will be included only in the console build
//--------------------------------------------------------------------------------------

void ASConsole::error(const char *why, const char* what) const
{
	(*_err) << why << ' ' << what << '\n' << endl;
	(*_err) << "Artistic Style has terminated!" << endl;
	exit(EXIT_FAILURE);
}

/**
 * If no files have been given, use cin for input and cout for output.
 *
 * This is used to format text for text editors like TextWrangler (Mac).
 * Do NOT display any console messages when this function is used.
 */
void ASConsole::formatCinToCout(ASFormatter& formatter) const
{
	ASStreamIterator<istream> streamIterator(&cin);     // create iterator for cin
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		cout << formatter.nextLine();
		if (formatter.hasMoreLines())
			cout << streamIterator.getOutputEOL();
	}
	cout.flush();
}

/**
 * Open input file, format it, and close the output.
 *
 * @param fileName      The path and name of the file to be processed.
 * @param formatter     The formatter object.
 * @return              true if the file was formatted, false if it was not (no changes).
 */
bool ASConsole::formatFile(const string &fileName, ASFormatter &formatter) const
{
	bool isFormatted = false;         // return value

	// open input file
	ifstream in(fileName.c_str(), ios::binary);
	if (!in)
		error("Could not open input file", fileName.c_str());

	ostringstream out;

	// Unless a specific language mode has been set, set the language mode
	// according to the file's suffix.
	if (!formatter.getModeManuallySet())
	{
		if (stringEndsWith(fileName, string(".java")))
			formatter.setJavaStyle();
		else if (stringEndsWith(fileName, string(".cs")))
			formatter.setSharpStyle();
		else
			formatter.setCStyle();
	}

	// format the file

	ASStreamIterator<istream> streamIterator(&in);
	formatter.init(&streamIterator);

	bool filesAreIdentical = true;   // input and output files are identical
	string nextLine;                 // next output line
	while (formatter.hasMoreLines())
	{
		nextLine = formatter.nextLine();
		out << nextLine;
		g_console->linesOut++;
		if (formatter.hasMoreLines())
			out << streamIterator.getOutputEOL();
		else
			streamIterator.saveLastInputLine();     // to compare the last input line

		if (filesAreIdentical)
		{
			if (streamIterator.checkForEmptyLine)
			{
				if (nextLine.find_first_not_of(" \t") != string::npos)
					filesAreIdentical = false;
			}
			else if (!streamIterator.compareToInputBuffer(nextLine))
				filesAreIdentical = false;
			streamIterator.checkForEmptyLine = false;
		}
	}
	in.close();

	// create output files

	// if file has changed, write the new file
	if (!filesAreIdentical)
	{
		// create a backup
		if (!noBackup)
		{
			string origFileName = fileName + origSuffix;
			removeFile(origFileName.c_str(), "Could not remove pre-existing backup file");
			renameFile(fileName.c_str(), origFileName.c_str(), "Could not create backup file");
		}

		// write the output file
		ofstream fout(fileName.c_str(), ios::binary | ios::trunc);
		if (!fout)
			error("Could not open output file", fileName.c_str());
		fout << out.str();
		fout.close();

		isFormatted = true;
	}

	return isFormatted;
}

#ifdef _WIN32  // Windows specific

/**
 * WINDOWS function to get the current directory.
 * NOTE: getenv("CD") does not work for Windows Vista.
 *        The Windows function GetCurrentDirectory is used instead.
 *
 * @return              The path of the current directory
 */
string ASConsole::getCurrentDirectory(const string &fileName) const
{
	char currdir[MAX_PATH];
	currdir[0] = '\0';
	if (!GetCurrentDirectory(sizeof(currdir), currdir))
		error("Cannot find file", fileName.c_str());
	return string(currdir);
}

#else  // not _WIN32

/**
 * LINUX function to get the current directory.
 * This is done if the fileName does not contain a path.
 * It is probably from an editor sending a single file.
 *
 * @param fileName      The filename is used only for  the error message.
 * @return              The path of the current directory
 */
string ASConsole::getCurrentDirectory(const string &fileName) const
{
	char *currdir = getenv("PWD");
	if (currdir == NULL)
		error("Cannot find file", fileName.c_str());
	return string(currdir);
}

#endif

#ifdef _WIN32  // Windows specific

/**
 * WINDOWS function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcard      The wildcard to be processed (e.g. *.cpp).
 * @param fileName      An empty vector which will be filled with the path and names of files to process.
 */
void ASConsole::getFileNames(const string &directory, const string &wildcard)
{
	vector<string> subDirectory;    // sub directories of directory
	WIN32_FIND_DATA FindFileData;   // for FindFirstFile and FindNextFile

	// Find the first file in the directory
	string firstFile = directory + "\\*";
	HANDLE hFind = FindFirstFile(firstFile.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		error("Cannot open directory", directory.c_str());

	// save files and sub directories
	do
	{
		// skip hidden or read only
		if (FindFileData.cFileName[0] == '.'
		        || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		        || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			continue;

		// if a sub directory and recursive, save sub directory
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && isRecursive)
		{
			string subDirectoryPath = directory + g_fileSeparator + FindFileData.cFileName;
			if (isPathExclued(subDirectoryPath))
			{
				if (!isQuiet)
					cout << "exclude " << subDirectoryPath.substr(mainDirectoryLength) << endl;
			}
			else
				subDirectory.push_back(subDirectoryPath);
			continue;
		}

		// save the file name
		string filePathName = directory + g_fileSeparator + FindFileData.cFileName;
		// check exclude before wildcmp to avoid "unmatched exclude" error
		bool isExcluded = isPathExclued(filePathName);
		// save file name if wildcard match
		if (wildcmp(wildcard.c_str(), FindFileData.cFileName))
		{
			if (isExcluded)
				cout << "exclude " << filePathName.substr(mainDirectoryLength) << endl;
			else
				fileName.push_back(filePathName);
		}
	}
	while (FindNextFile(hFind, &FindFileData) != 0);

	// check for processing error
	FindClose(hFind);
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		error("Error processing directory", directory.c_str());

	// recurse into sub directories
	// if not doing recursive subDirectory is empty
	for (unsigned i = 0; i < subDirectory.size(); i++)
	{
		getFileNames(subDirectory[i], wildcard);
	}

	return;
}

#else  // not _WIN32

/**
 * LINUX function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcard      The wildcard to be processed (e.g. *.cpp).
 * @param fileName      An empty vector which will be filled with the path and names of files to process.
 */
void ASConsole::getFileNames(const string &directory, const string &wildcard)
{
	struct dirent *entry;           // entry from readdir()
	struct stat statbuf;            // entry from stat()
	vector<string> subDirectory;    // sub directories of this directory

	// errno is defined in <errno.h> and is set for errors in opendir, readdir, or stat
	errno = 0;

	DIR *dp = opendir(directory.c_str());
	if (errno)
		error("Cannot open directory", directory.c_str());

	// save the first fileName entry for this recursion
	const unsigned firstEntry = fileName.size();

	// save files and sub directories
	while ((entry = readdir(dp)) != NULL)
	{
		// get file status
		string entryFilepath = directory + g_fileSeparator + entry->d_name;
		stat(entryFilepath.c_str(), &statbuf);
		if (errno)
		{
			if (errno == EOVERFLOW)         // file over 2 GB is OK
			{
				errno = 0;
				continue;
			}
			perror("errno message");
			error("Error getting file status in directory", directory.c_str());
		}
		// skip hidden or read only
		if (entry->d_name[0] == '.' || !(statbuf.st_mode & S_IWUSR))
			continue;
		// if a sub directory and recursive, save sub directory
		if (S_ISDIR(statbuf.st_mode) && isRecursive)
		{
			if (isPathExclued(entryFilepath))
				cout << "exclude " << entryFilepath.substr(mainDirectoryLength) << endl;
			else
				subDirectory.push_back(entryFilepath);
			continue;
		}

		// if a file, save file name
		if (S_ISREG(statbuf.st_mode))
		{
			// check exclude before wildcmp to avoid "unmatched exclude" error
			bool isExcluded = isPathExclued(entryFilepath);
			// save file name if wildcard match
			if (wildcmp(wildcard.c_str(), entry->d_name))
			{
				if (isExcluded)
					cout << "exclude " << entryFilepath.substr(mainDirectoryLength) << endl;
				else
					fileName.push_back(entryFilepath);
			}
		}
	}
	closedir(dp);

	if (errno)
	{
		perror("errno message");
		error("Error reading directory", directory.c_str());
	}

	// sort the current entries for fileName
	if (firstEntry < fileName.size())
		sort(&fileName[firstEntry], &fileName[fileName.size()]);

	// recurse into sub directories
	// if not doing recursive, subDirectory is empty
	if (subDirectory.size() > 1)
		sort(subDirectory.begin(), subDirectory.end());
	for (unsigned i = 0; i < subDirectory.size(); i++)
	{
		getFileNames(subDirectory[i], wildcard);
	}

	return;
}

#endif  // _WIN32

// compare a path to the exclude vector
// used for both directories and filenames
// updates the g_excludeHitsVector
// return true if a match
bool ASConsole::isPathExclued(const string &subPath)
{
	bool retVal = false;

	// read the exclude vector checking for a match
	for (size_t i = 0; i < excludeVector.size(); i++)
	{
		string exclude = excludeVector[i];

		if (subPath.length() > exclude.length())
		{
			size_t compareStart = subPath.length() - exclude.length();
			char lastPathChar = subPath[compareStart - 1];

			// exclude must start with a directory name
			if (lastPathChar == g_fileSeparator)
			{
				string compare = subPath.substr(compareStart);
				if (!g_isCaseSensitive)
				{
					// make it case insensitive for Windows
					for (size_t j=0; j<compare.length(); j++)
						compare[j] = (char)tolower(compare[j]);
					for (size_t j=0; j<exclude.length(); j++)
						exclude[j] = (char)tolower(exclude[j]);
				}
				// compare sub directory to exclude data - must check them all
				if (compare == exclude)
				{
					excludeHitsVector[i] = true;
					retVal = true;
					break;
				}
			}
		}
	}
	return retVal;
}

void ASConsole::printHelp() const
{
	(*_err) << endl;
	(*_err) << "                            Artistic Style " << _version << endl;
	(*_err) << "                         Maintained by: Jim Pattee\n";
	(*_err) << "                       Original Author: Tal Davidson\n";
	(*_err) << endl;
	(*_err) << "Usage  :  astyle [options] Source1.cpp Source2.cpp  [...]\n";
	(*_err) << "          astyle [options] < Original > Beautified\n";
	(*_err) << endl;
	(*_err) << "When indenting a specific file, the resulting indented file RETAINS the\n";
	(*_err) << "original file-name. The original pre-indented file is renamed, with a\n";
	(*_err) << "suffix of \".orig\" added to the original filename.\n";
	(*_err) << endl;
	(*_err) << "Wildcards (* and ?) may be used in the filename.\n";
	(*_err) << "A \'recursive\' option can process directories recursively.\n";
	(*_err) << endl;
	(*_err) << "By default, astyle is set up to indent C/C++/C#/Java files, with four\n";
	(*_err) << "spaces per indent, a maximal indentation of 40 spaces inside continuous\n";
	(*_err) << "statements, a minimum indentation of eight spaces inside conditional\n";
	(*_err) << "statements, and NO formatting options.\n";
	(*_err) << endl;
	(*_err) << "Option's Format:\n";
	(*_err) << "----------------\n";
	(*_err) << "    Long options (starting with '--') must be written one at a time.\n";
	(*_err) << "    Short options (starting with '-') may be appended together.\n";
	(*_err) << "    Thus, -bps4 is the same as -b -p -s4.\n";
	(*_err) << endl;
	(*_err) << "Predefined Style Options:\n";
	(*_err) << "-------------------------\n";
	(*_err) << "    --style=allman  OR  --style=ansi  OR  --style=bsd  OR  -A1\n";
	(*_err) << "    Allman style formatting/indenting.\n";
	(*_err) << "    Broken brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=java  OR  -A2\n";
	(*_err) << "    Java style formatting/indenting.\n";
	(*_err) << "    Attached brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=k&r  OR  --style=k/r  OR  -A3\n";
	(*_err) << "    Kernighan & Ritchie style formatting/indenting.\n";
	(*_err) << "    Linux brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=stroustrup  OR  -A4\n";
	(*_err) << "    Stroustrup style formatting/indenting.\n";
	(*_err) << "    Stroustrup brackets.\n";
	(*_err) << endl;
	(*_err) << "    --style=whitesmith  OR  -A5\n";
	(*_err) << "    Whitesmith style formatting/indenting.\n";
	(*_err) << "    Broken, indented brackets.\n";
	(*_err) << "    Indented class blocks and switch blocks.\n";
	(*_err) << endl;
	(*_err) << "    --style=banner  OR  -A6\n";
	(*_err) << "    Banner style formatting/indenting.\n";
	(*_err) << "    Attached, indented brackets.\n";
	(*_err) << "    Indented class blocks and switch blocks.\n";
	(*_err) << endl;
	(*_err) << "    --style=gnu  OR  -A7\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << "    Broken brackets, indented blocks, indent is 2 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=linux  OR  -A8\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << "    Linux brackets, indent is 8 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=horstmann  OR  -A9\n";
	(*_err) << "    Horstmann style formatting/indenting.\n";
	(*_err) << "    Horstmann brackets, indented switches, indent is 3 spaces.\n";
	(*_err) << endl;
	(*_err) << "    --style=1tbs  OR  --style=otbs  OR  -A10\n";
	(*_err) << "    One True Brace Style formatting/indenting.\n";
	(*_err) << "    Linux brackets, add brackets to all conditionals.\n";
	(*_err) << endl;
	(*_err) << "Tab and Bracket Options:\n";
	(*_err) << "------------------------\n";
	(*_err) << "    default indent option\n";
	(*_err) << "    If no indentation option is set,\n";
	(*_err) << "    the default option of 4 spaces will be used.\n";
	(*_err) << endl;
	(*_err) << "    --indent=spaces=#  OR  -s#\n";
	(*_err) << "    Indent using # spaces per indent. Not specifying #\n";
	(*_err) << "    will result in a default of 4 spaces per indent.\n";
	(*_err) << endl;
	(*_err) << "    --indent=tab  OR  --indent=tab=#  OR  -t  OR  -t#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Not specifying # will result\n";
	(*_err) << "    in a default assumption of 4 spaces per tab.\n";
	(*_err) << endl;
	(*_err) << "    --indent=force-tab=#  OR  -T#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Force tabs to be used in areas\n";
	(*_err) << "    Astyle would prefer to use spaces.\n";
	(*_err) << endl;
	(*_err) << "    default brackets option\n";
	(*_err) << "    If no brackets option is set,\n";
	(*_err) << "    the brackets will not be changed.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break  OR  -b\n";
	(*_err) << "    Break brackets from pre-block code (i.e. ANSI C/C++ style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=attach  OR  -a\n";
	(*_err) << "    Attach brackets to pre-block code (i.e. Java/K&R style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=linux  OR  -l\n";
	(*_err) << "    Break definition-block brackets and attach command-block\n";
	(*_err) << "    brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=stroustrup  OR  -u\n";
	(*_err) << "    Attach all brackets except function definition brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=horstmann  OR  -g\n";
	(*_err) << "    Break brackets from pre-block code, but allow following\n";
	(*_err) << "    run-in statements on the same line as an opening bracket.\n";
	(*_err) << endl;
	(*_err) << "Indentation options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --indent-classes  OR  -C\n";
	(*_err) << "    Indent 'class' blocks, so that the inner 'public:',\n";
	(*_err) << "    'protected:' and 'private: headers are indented in\n";
	(*_err) << "    relation to the class block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-switches  OR  -S\n";
	(*_err) << "    Indent 'switch' blocks, so that the inner 'case XXX:'\n";
	(*_err) << "    headers are indented in relation to the switch block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-cases  OR  -K\n";
	(*_err) << "    Indent case blocks from the 'case XXX:' headers.\n";
	(*_err) << "    Case statements not enclosed in blocks are NOT indented.\n";
	(*_err) << endl;
	(*_err) << "    --indent-brackets  OR  -B\n";
	(*_err) << "    Add extra indentation to '{' and '}' block brackets.\n";
	(*_err) << endl;
	(*_err) << "    --indent-blocks  OR  -G\n";
	(*_err) << "    Add extra indentation entire blocks (including brackets).\n";
	(*_err) << endl;
	(*_err) << "    --indent-namespaces  OR  -N\n";
	(*_err) << "    Indent the contents of namespace blocks.\n";
	(*_err) << endl;
	(*_err) << "    --indent-labels  OR  -L\n";
	(*_err) << "    Indent labels so that they appear one indent less than\n";
	(*_err) << "    the current indentation level, rather than being\n";
	(*_err) << "    flushed completely to the left (which is the default).\n";
	(*_err) << endl;
	(*_err) << "    --indent-preprocessor  OR  -w\n";
	(*_err) << "    Indent multi-line #define statements.\n";
	(*_err) << endl;
	(*_err) << "    --max-instatement-indent=#  OR  -M#\n";
	(*_err) << "    Indent a maximal # spaces in a continuous statement,\n";
	(*_err) << "    relative to the previous line.\n";
	(*_err) << endl;
	(*_err) << "    --min-conditional-indent=#  OR  -m#\n";
	(*_err) << "    Indent a minimal # spaces in a continuous conditional\n";
	(*_err) << "    belonging to a conditional header.\n";
	(*_err) << endl;
	(*_err) << "Padding options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --break-blocks  OR  -f\n";
	(*_err) << "    Insert empty lines around unrelated blocks, labels, classes, ...\n";
	(*_err) << endl;
	(*_err) << "    --break-blocks=all  OR  -F\n";
	(*_err) << "    Like --break-blocks, except also insert empty lines \n";
	(*_err) << "    around closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << endl;
	(*_err) << "    --pad-oper  OR  -p\n";
	(*_err) << "    Insert space paddings around operators.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren  OR  -P\n";
	(*_err) << "    Insert space padding around parenthesis on both the outside\n";
	(*_err) << "    and the inside.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren-out  OR  -d\n";
	(*_err) << "    Insert space padding around parenthesis on the outside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad-paren-in  OR  -D\n";
	(*_err) << "    Insert space padding around parenthesis on the inside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad-header  OR  -H\n";
	(*_err) << "    Insert space padding after paren headers (e.g. 'if', 'for'...).\n";
	(*_err) << endl;
	(*_err) << "    --unpad-paren  OR  -U\n";
	(*_err) << "    Remove unnecessary space padding around parenthesis.  This\n";
	(*_err) << "    can be used in combination with the 'pad' options above.\n";
	(*_err) << endl;
	(*_err) << "    --delete-empty-lines  OR  -x\n";
	(*_err) << "    Delete empty lines within a function or method.\n";
	(*_err) << "    It will NOT delete lines added by the break-blocks options.\n";
	(*_err) << endl;
	(*_err) << "    --fill-empty-lines  OR  -E\n";
	(*_err) << "    Fill empty lines with the white space of their\n";
	(*_err) << "    previous lines.\n";
	(*_err) << endl;
	(*_err) << "Formatting options:\n";
	(*_err) << "-------------------\n";
	(*_err) << "    --break-closing-brackets  OR  -y\n";
	(*_err) << "    Break brackets before closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << "    Use with --brackets=attach, --brackets=linux, \n";
	(*_err) << "    or --brackets=stroustrup.\n";
	(*_err) << endl;
	(*_err) << "    --break-elseifs  OR  -e\n";
	(*_err) << "    Break 'else if()' statements into two different lines.\n";
	(*_err) << endl;
	(*_err) << "    --keep-one-line-blocks  OR  -O\n";
	(*_err) << "    Don't break blocks residing completely on one line.\n";
	(*_err) << endl;
	(*_err) << "    --keep-one-line-statements  OR  -o\n";
	(*_err) << "    Don't break lines containing multiple statements into\n";
	(*_err) << "    multiple single-statement lines.\n";
	(*_err) << endl;
	(*_err) << "    --align-pointer=type    OR  -k1\n";
	(*_err) << "    --align-pointer=middle  OR  -k2\n";
	(*_err) << "    --align-pointer=name    OR  -k3\n";
	(*_err) << "    Attach a pointer or reference operator (* or &) to either\n";
	(*_err) << "    the operator type (left), middle, or operator name (right).\n";
	(*_err) << endl;
	(*_err) << "    --convert-tabs  OR  -c\n";
	(*_err) << "    Convert tabs to the appropriate number of spaces.\n";
	(*_err) << endl;
	(*_err) << "    --mode=c\n";
	(*_err) << "    Indent a C or C++ source file (this is the default).\n";
	(*_err) << endl;
	(*_err) << "    --mode=java\n";
	(*_err) << "    Indent a Java source file.\n";
	(*_err) << endl;
	(*_err) << "    --mode=cs\n";
	(*_err) << "    Indent a C# source file.\n";
	(*_err) << endl;
	(*_err) << "Other options:\n";
	(*_err) << "--------------\n";
	(*_err) << "    --suffix=####\n";
	(*_err) << "    Append the suffix #### instead of '.orig' to original filename.\n";
	(*_err) << endl;
	(*_err) << "    --suffix=none  OR  -n\n";
	(*_err) << "    Do not retain a backup of the original file.\n";
	(*_err) << endl;
	(*_err) << "    --options=####\n";
	(*_err) << "    Specify an options file #### to read and use.\n";
	(*_err) << endl;
	(*_err) << "    --options=none\n";
	(*_err) << "    Disable the default options file.\n";
	(*_err) << "    Only the command-line parameters will be used.\n";
	(*_err) << endl;
	(*_err) << "    --recursive  OR  -r  OR  -R\n";
	(*_err) << "    Process subdirectories recursively.\n";
	(*_err) << endl;
	(*_err) << "    --exclude=####\n";
	(*_err) << "    Specify a file or directory #### to be excluded from processing.\n";
	(*_err) << endl;
	(*_err) << "    --errors-to-stdout  OR  -X\n";
	(*_err) << "    Print errors and help information to standard-output rather than\n";
	(*_err) << "    to standard-error.\n";
	(*_err) << endl;
	(*_err) << "    --verbose  OR  -v\n";
	(*_err) << "    Verbose mode. Extra informational messages will be displayed.\n";
	(*_err) << endl;
	(*_err) << "    --formatted  OR  -Q\n";
	(*_err) << "    Formatted display mode. Display only the files that have been formatted.\n";
	(*_err) << endl;
	(*_err) << "    --quiet  OR  -q\n";
	(*_err) << "    Quiet mode. Suppress all output except error messages.\n";
	(*_err) << endl;
	(*_err) << "    --version  OR  -V\n";
	(*_err) << "    Print version number.\n";
	(*_err) << endl;
	(*_err) << "    --help  OR  -h  OR  -?\n";
	(*_err) << "    Print this help message.\n";
	(*_err) << endl;
	(*_err) << "Default options file:\n";
	(*_err) << "---------------------\n";
	(*_err) << "    Artistic Style looks for a default options file in the\n";
	(*_err) << "    following order:\n";
	(*_err) << "    1. The contents of the ARTISTIC_STYLE_OPTIONS environment\n";
	(*_err) << "       variable if it exists.\n";
	(*_err) << "    2. The file called .astylerc in the directory pointed to by the\n";
	(*_err) << "       HOME environment variable ( i.e. $HOME/.astylerc ).\n";
	(*_err) << "    3. The file called astylerc in the directory pointed to by the\n";
	(*_err) << "       USERPROFILE environment variable ( i.e. %USERPROFILE%\\astylerc ).\n";
	(*_err) << "    If a default options file is found, the options in this file\n";
	(*_err) << "    will be parsed BEFORE the command-line options.\n";
	(*_err) << "    Long options within the default option file may be written without\n";
	(*_err) << "    the preliminary '--'.\n";
	(*_err) << endl;
}

// get individual file names from the command-line file path
void ASConsole::getFilePaths(string &filePath)
{
//	vector<string> fileName;        // files to be processed including path
//	string targetDirectory;         // path to the directory being processed
//	string targetFilename;          // file name being processed

	// standardize the file separators
//	standardizePath(filePath);

	fileName.clear();
	targetDirectory = string();
	targetFilename = string();

	// separate directory and file name
	size_t separator = filePath.find_last_of(g_fileSeparator);
	if (separator == string::npos)
	{
		// if no directory is present, use the currently active directory
		targetDirectory = getCurrentDirectory(filePath);
		targetFilename  = filePath;
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}
	else
	{
		targetDirectory = filePath.substr(0, separator);
		targetFilename  = filePath.substr(separator+1);
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}

	if (targetFilename.length() == 0)
		error("Missing filename in", filePath.c_str());

	// check filename for wildcards
	hasWildcard = false;
	if (targetFilename.find_first_of( "*?") != string::npos)
		hasWildcard = true;

	// clear exclude hits vector
	for (size_t ix = 0; ix < excludeHitsVector.size(); ix++)
		excludeHitsVector[ix] = false;

	// display directory name for wildcard processing
	if (hasWildcard && ! isQuiet)
	{
		cout << "--------------------------------------------------" << endl;
		cout << "directory " << targetDirectory << g_fileSeparator << targetFilename <<  endl;
	}

	// create a vector of paths and file names to process
	if (hasWildcard || isRecursive)
		getFileNames(targetDirectory, targetFilename);
	else
		fileName.push_back(targetDirectory + g_fileSeparator + targetFilename);

	if (hasWildcard && ! isQuiet)
		cout << "--------------------------------------------------" << endl;

	// check for unprocessed excludes
	bool excludeErr = false;
	for (size_t ix = 0; ix < excludeHitsVector.size(); ix++)
	{
		if (excludeHitsVector[ix] == false)
		{
			(*_err) << "Unmatched exclude " << excludeVector[ix].c_str() << endl;
			excludeErr = true;
		}
	}
	if (excludeErr)
		exit(EXIT_FAILURE);

	// check if files were found (probably an input error if not)
	if (fileName.size() == 0)
		(*_err) << "No file to process " << filePath.c_str() << endl;
}

// process options from the command line and options file
// build the vectors fileNameVector, excludeVector, optionsVector, and fileOptionsVector
processReturn ASConsole::processOptions(int argc, char** argv, ASFormatter &formatter)
{
	string arg;
	bool ok = true;
	bool shouldParseOptionsFile = true;

	// get command line options
	for (int i = 1; i < argc; i++)
	{
		arg = string(argv[i]);

		if ( IS_OPTION(arg, "--options=none") )
		{
			shouldParseOptionsFile = false;
		}
		else if ( isParamOption(arg, "--options=") )
		{
			optionsFileName = GET_PARAM(arg, "--options=");
			optionsFileRequired = true;
			if (optionsFileName.compare("") == 0)
				optionsFileName = ' ';
		}
		else if ( IS_OPTION(arg, "-h")
		          || IS_OPTION(arg, "--help")
		          || IS_OPTION(arg, "-?") )
		{
			printHelp();
			return(END_SUCCESS);
		}
		else if ( IS_OPTION(arg, "-V" )
		          || IS_OPTION(arg, "--version") )
		{
			(*_err) << "Artistic Style Version " << _version << endl;
			return(END_SUCCESS);
		}
		else if (arg[0] == '-')
		{
			optionsVector.push_back(arg);
		}
		else // file-name
		{
			g_console->standardizePath(arg);
			fileNameVector.push_back(arg);
		}
	}

	// get options file path and name
	if (shouldParseOptionsFile)
	{
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("ARTISTIC_STYLE_OPTIONS");
			if (env != NULL)
				optionsFileName = string(env);
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("HOME");
			if (env != NULL)
				optionsFileName = string(env) + string("/.astylerc");
		}
		if (optionsFileName.compare("") == 0)
		{
			char* env = getenv("USERPROFILE");
			if (env != NULL)
				optionsFileName = string(env) + string("/astylerc");
		}
		if (optionsFileName.compare("") != 0)
			standardizePath(optionsFileName);
	}

	// create the options file vector and parse the options for errors
	if (optionsFileName.compare("") != 0)
	{
		ifstream optionsIn(optionsFileName.c_str());
		if (optionsIn)
		{
			importOptions(optionsIn, fileOptionsVector);
			ok = parseOptions(formatter,
			                  fileOptionsVector.begin(),
			                  fileOptionsVector.end(),
			                  string("Invalid option in default options file: "));
		}
		else
		{
			if (optionsFileRequired)
			{
				(*_err) << "Could not open options file: " << optionsFileName.c_str() << endl;
				return (END_FAILURE);
			}
			optionsFileName.clear();
		}
		optionsIn.close();
	}
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' " << endl;
		return(END_FAILURE);
	}

	// parse the command line options vector for errors
	ok = parseOptions(formatter,
	                  optionsVector.begin(),
	                  optionsVector.end(),
	                  string("Invalid command line option: "));
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' \n" << endl;
		return(END_FAILURE);
	}
	return(CONTINUE);
}

// remove a file and check for an error
void ASConsole::removeFile(const char* fileName, const char* errMsg) const
{
	remove(fileName);
	if (errno == ENOENT)        // no file is OK
		errno = 0;
	if (errno)
	{
		perror("errno message");
		error(errMsg, fileName);
	}
}

// rename a file and check for an error
void ASConsole::renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const
{
	rename(oldFileName, newFileName);
	// if file still exists the remove needs more time - retry
	if (errno == EEXIST)
	{
		errno = 0;
		waitForRemove(newFileName);
		rename(oldFileName, newFileName);
	}
	if (errno)
	{
		perror("errno message");
		error(errMsg, oldFileName);
	}
}

// make sure file separators are correct type (Windows or Linux)
// remove ending file separator
// remove beginning file separator if requested and NOT a complete file path
void ASConsole::standardizePath(string &path, bool removeBeginningSeparator /*false*/) const
{
#ifdef __VMS
	struct FAB fab;
	struct NAML naml;
	char less[NAML$C_MAXRSS];
	char sess[NAM$C_MAXRSS];
	int r0_status;

	// If we are on a VMS system, translate VMS style filenames to unix
	// style.
	fab = cc$rms_fab;
	fab.fab$l_fna = (char *)-1;
	fab.fab$b_fns = 0;
	fab.fab$l_naml = &naml;
	naml = cc$rms_naml;
	strcpy (sess, path.c_str());
	naml.naml$l_long_filename = (char *)sess;
	naml.naml$l_long_filename_size = path.length();
	naml.naml$l_long_expand = less;
	naml.naml$l_long_expand_alloc = sizeof (less);
	naml.naml$l_esa = sess;
	naml.naml$b_ess = sizeof (sess);
	naml.naml$v_no_short_upcase = 1;
	r0_status = sys$parse (&fab);
	if (r0_status == RMS$_SYN)
	{
		error("File syntax error", path.c_str());
	}
	else
	{
		if (!$VMS_STATUS_SUCCESS(r0_status))
		{
			(void)lib$signal (r0_status);
		}
	}
	less[naml.naml$l_long_expand_size - naml.naml$b_ver] = '\0';
	sess[naml.naml$b_esl - naml.naml$b_ver] = '\0';
	if (naml.naml$l_long_expand_size > naml.naml$b_esl)
	{
		path = decc$translate_vms (less);
	}
	else
	{
		path = decc$translate_vms (sess);
	}
#endif /* __VMS */

	// make sure separators are correct type (Windows or Linux)
	for (size_t i = 0; i < path.length(); i++)
	{
		i = path.find_first_of("/\\", i);
		if (i == string::npos)
			break;
		path[i] = g_fileSeparator;
	}
	// remove separator from the end
	if (path[path.length()-1] == g_fileSeparator)
		path.erase(path.length()-1, 1);
	// remove beginning separator if requested
	if (removeBeginningSeparator && (path[0] == g_fileSeparator))
		path.erase(0, 1);
}

bool ASConsole::stringEndsWith(const string &str, const string &suffix) const
{
	int strIndex = (int) str.length() - 1;
	int suffixIndex = (int) suffix.length() - 1;

	while (strIndex >= 0 && suffixIndex >= 0)
	{
		if (tolower(str[strIndex]) != tolower(suffix[suffixIndex]))
			return false;

		--strIndex;
		--suffixIndex;
	}

	return true;
}

void ASConsole::wait(int seconds) const
{
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {}
}

int ASConsole::waitForRemove(const char* newFileName) const
{
	struct stat stBuf;
	int seconds;
	// wait a max of 20 seconds for the remove
	for (seconds = 0; seconds < 20; seconds++)
	{
		wait(1);
		if (stat(newFileName, &stBuf) != 0)
			break;
	}
	errno = 0;
	return seconds;
}

// From The Code Project http://www.codeproject.com/string/wildcmp.asp
// Written by Jack Handy - jakkhandy@hotmail.com
// Modified to compare case insensitive for Windows
int ASConsole::wildcmp(const char *wild, const char *data) const
{
	const char *cp = NULL, *mp = NULL;
	bool cmpval;

	while ((*data) && (*wild != '*'))
	{
		if (!g_isCaseSensitive)
			cmpval = (tolower(*wild) != tolower(*data)) && (*wild != '?');
		else
			cmpval = (*wild != *data) && (*wild != '?');

		if (cmpval)
		{
			return 0;
		}
		wild++;
		data++;
	}

	while (*data)
	{
		if (*wild == '*')
		{
			if (!*++wild)
			{
				return 1;
			}
			mp = wild;
			cp = data+1;
		}
		else
		{
			if (!g_isCaseSensitive)
				cmpval = (tolower(*wild) == tolower(*data) || (*wild == '?'));
			else
				cmpval = (*wild == *data) || (*wild == '?');

			if (cmpval)
			{
				wild++;
				data++;
			}
			else
			{
				wild = mp;
				data = cp++;
			}
		}
	}

	while (*wild == '*')
	{
		wild++;
	}
	return !*wild;
}

#endif
// *******************   end of console functions   ***********************************************

}   // end of namespace astyle

// *******************   end of astyle namespace    ***********************************************

using namespace astyle;

#ifdef ASTYLE_JNI
// *************************   JNI functions   *****************************************************
// called by a java program to get the version number
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass)
{
	return env->NewStringUTF(_version);
}

// called by a java program to format the source code
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain(JNIEnv* env,
        jobject obj,
        jstring textInJava,
        jstring optionsJava)
{
	g_env = env;                                // make object available globally
	g_obj = obj;                                // make object available globally

	jstring textErr = env->NewStringUTF("");    // zero length text returned if an error occurs

	// get the method ID
	jclass cls = env->GetObjectClass(obj);
	g_mid = env->GetMethodID(cls, "ErrorHandler","(ILjava/lang/String;)V");
	if (g_mid == 0)
	{
		cout << "Cannot find java method ErrorHandler" << endl;
		return textErr;
	}

	// convert jstring to char*
	const char* textIn = env->GetStringUTFChars(textInJava, NULL);
	const char* options = env->GetStringUTFChars(optionsJava, NULL);

	// call the C++ formatting function
	char* textOut = AStyleMain(textIn, options, javaErrorHandler, javaMemoryAlloc);
	// if an error message occurred it was displayed by errorHandler
	if (textOut == NULL)
		return textErr;

	// release memory
	jstring textOutJava = env->NewStringUTF(textOut);
	delete [] textOut;
	env->ReleaseStringUTFChars(textInJava, textIn);
	env->ReleaseStringUTFChars(optionsJava, options);

	return textOutJava;
}

// Call the Java error handler
void STDCALL javaErrorHandler(int errorNumber, char* errorMessage)
{
	jstring errorMessageJava = g_env->NewStringUTF(errorMessage);
	g_env->CallVoidMethod(g_obj, g_mid, errorNumber, errorMessageJava);
}

// Allocate memory for the formatted text
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded)
{
	// error condition is checked after return from AStyleMain
	char* buffer = new(nothrow) char [memoryNeeded];
	return buffer;
}
#endif

#ifdef ASTYLE_LIB
// *************************   GUI functions   ****************************************************
/*
 * IMPORTANT VC DLL linker must have the parameter  /EXPORT:AStyleMain=_AStyleMain@16
 *                                                  /EXPORT:AStyleGetVersion=_AStyleGetVersion@0
 */
extern "C" EXPORT char* STDCALL
AStyleMain(const char* pSourceIn,          // pointer to the source to be formatted
           const char* pOptions,           // pointer to AStyle options, separated by \n
           fpError fpErrorHandler,         // pointer to error handler function
           fpAlloc fpMemoryAlloc)          // pointer to memory allocation function
{
	if (fpErrorHandler == NULL)         // cannot display a message if no error handler
		return NULL;

	if (pSourceIn == NULL)
	{
		fpErrorHandler(101, (char*)"No pointer to source input.");
		return NULL;
	}
	if (pOptions == NULL)
	{
		fpErrorHandler(102, (char*)"No pointer to AStyle options.");
		return NULL;
	}
	if (fpMemoryAlloc == NULL)
	{
		fpErrorHandler(103, (char*)"No pointer to memory allocation function.");
		return NULL;
	}

	ASFormatter formatter;

	vector<string> optionsVector;
	istringstream opt(pOptions);
	_err = new stringstream;

	importOptions(opt, optionsVector);

	parseOptions(formatter,
	             optionsVector.begin(),
	             optionsVector.end(),
	             "Invalid Artistic Style options.\n"
	             "The following options were not processed:");

	if (_err->str().length() > 0)
		fpErrorHandler(210, (char*) _err->str().c_str());

	delete _err;
	_err = NULL;

	istringstream in(pSourceIn);
	ASStreamIterator<istringstream> streamIterator(&in);
	ostringstream out;
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		out << formatter.nextLine();
		if (formatter.hasMoreLines())
			out << streamIterator.getOutputEOL();
	}

	unsigned long textSizeOut = out.str().length();
	char* pTextOut = fpMemoryAlloc(textSizeOut + 1);     // call memory allocation function
//    pTextOut = NULL;           // for testing
	if (pTextOut == NULL)
	{
		fpErrorHandler(110, (char*)"Allocation failure on output.");
		return NULL;
	}

	strcpy(pTextOut, out.str().c_str());

	return pTextOut;
}

extern "C" EXPORT const char* STDCALL AStyleGetVersion (void)
{
	return _version;
}

// ASTYLE_NO_MAIN is defined to exclude "main" from the test programs
#elif !defined(ASTYLE_NO_MAIN)

// **************************   main function   ***************************************************

int main(int argc, char** argv)
{
	ASFormatter formatter;
	g_console = new ASConsole;

	// process command line and options file
	// build the vectors fileNameVector, optionsVector, and fileOptionsVector
	processReturn returnValue = g_console->processOptions(argc, argv, formatter);

	if (returnValue == END_SUCCESS)
		return EXIT_SUCCESS;
	if (returnValue == END_FAILURE)
	{
		(*_err) << "Artistic Style has terminated!" << endl;
		return EXIT_FAILURE;
	}

	// if no files have been given, use cin for input and cout for output
	if (g_console->fileNameVector.empty())
	{
		g_console->formatCinToCout(formatter);
		return EXIT_SUCCESS;
	}

	// indent the given files

	if (g_console->isVerbose)
	{
		cout << "Artistic Style " << _version << endl;
		if (g_console->optionsFileName.compare("") != 0)
			cout << "Using default options file " << g_console->optionsFileName << endl;
	}

	clock_t startTime = clock();     // start time of file formatting

	// loop thru input fileNameVector and process the files
	for (size_t i = 0; i < g_console->fileNameVector.size(); i++)
	{
		g_console->getFilePaths(g_console->fileNameVector[i]);

		// loop thru fileName vector formatting the files
		for (size_t j = 0; j < g_console->fileName.size(); j++)
		{
			// format the file
			bool isFormatted = g_console->formatFile(g_console->fileName[j], formatter);

			// remove targetDirectory from filename if required
			string displayName;
			if (g_console->hasWildcard)
				displayName = g_console->fileName[j].substr(g_console->targetDirectory.length() + 1);
			else
				displayName = g_console->fileName[j];

			if (isFormatted)
			{
				g_console->filesFormatted++;
				if (!g_console->isQuiet)
					cout << "formatted  " << displayName.c_str() << endl;
			}
			else
			{
				g_console->filesUnchanged++;
				if (!g_console->isQuiet && !g_console->isFormattedOnly)
					cout << "unchanged* " << displayName.c_str() << endl;
			}
		}
	}

	// files are processed, display stats
	if (g_console->isVerbose)
	{
		if (g_console->hasWildcard)
			cout << "--------------------------------------------------" << endl;
		cout << g_console->filesFormatted << " formatted, ";
		cout << g_console->filesUnchanged << " unchanged, ";

		// show processing time
		clock_t stopTime = clock();
		float secs = (float) (stopTime - startTime) / CLOCKS_PER_SEC;
		if (secs < 60)
		{
			// show tenths of a second if time is less than 20 seconds
			cout.precision(2);
			if (secs >= 10 && secs < 20)
				cout.precision(3);
			cout << secs << " seconds, ";
			cout.precision(0);
		}
		else
		{
			// show minutes and seconds if time is greater than one minute
			int min = (int) secs / 60;
			secs -= min * 60;
			int minsec = int (secs + .5);
			cout << min << " min " << minsec << " sec, ";
		}

		cout << g_console->linesOut << " lines" << endl;
	}

	delete g_console;
	return EXIT_SUCCESS;
}

#endif
