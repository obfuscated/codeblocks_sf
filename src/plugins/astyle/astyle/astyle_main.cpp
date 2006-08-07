/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   astyle_main.cpp
 *
 *   This file is a part of "Artistic Style" - an indentation and
 *   reformatting tool for C, C++, C# and Java source files.
 *   http://astyle.sourceforge.net
 *
 *   The "Artistic Style" project, including all files needed to compile
 *   it, is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA  02110-1301, USA.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "astyle.h"

#include <iostream>
#include <fstream>
#include <sstream>

// for G++ implementation of string.compare:
// compare((str), (place), (length))  instead of  compare(place, length, str)
#if defined(__GNUC__) && __GNUC__ < 3
#error - Use GNU C compiler release 3 or higher
#endif

// for namespace problem in version 5.0
#if defined(_MSC_VER) && _MSC_VER < 1200        // check for V6.0
#error - Use Microsoft compiler version 6 or higher
#endif

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT  __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

#define IS_OPTION(arg,op)          ((arg).compare(op)==0)
#define IS_OPTIONS(arg,a,b)        (IS_OPTION((arg),(a)) || IS_OPTION((arg),(b)))

#define GET_PARAM(arg,op)          ((arg).substr(strlen(op)))
#define GET_PARAMS(arg,a,b) (isParamOption((arg),(a)) ? GET_PARAM((arg),(a)) : GET_PARAM((arg),(b)))

using namespace astyle;

const string _version = "1.19";

// some compilers want this declared
bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo);

#if defined(ASTYLE_GUI) || defined(ASTYLE_DLL)
// GUI function pointers
typedef void (STDCALL *fpError)(int, char*);       // pointer to callback error handler
typedef char* (STDCALL *fpAlloc)(unsigned long);   // pointer to callback memory allocation
// GUI variables
stringstream *_err = NULL;
#else
// console variables
ostream *_err = &cerr;
string _suffix = ".orig";
#endif
bool _modeManuallySet;


// typename will be istringstream for GUI and istream otherwise
template<typename T>
class ASStreamIterator :
			public ASSourceIterator
{
	public:
		ASStreamIterator(T *in);
		virtual ~ASStreamIterator();
		bool hasMoreLines() const;
		string nextLine();

	private:
		T * inStream;
		string buffer;
};

template<typename T>
ASStreamIterator<T>::ASStreamIterator(T *in)
{
	inStream = in;
	buffer.reserve(200);
}


template<typename T>
ASStreamIterator<T>::~ASStreamIterator()
{
// NEW operator has been removed
//    delete inStream;
}


template<typename T>
bool ASStreamIterator<T>::hasMoreLines() const
{
	return !inStream->eof();
}


/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */
template<typename T>
string ASStreamIterator<T>::nextLine()
{
	getline(*inStream, buffer);
	size_t lineLength = buffer.length();

	if (lineLength > 0 && buffer[lineLength-1] == '\r')
	{
		eolWindows++;
		buffer.erase(lineLength - 1);
	}
	else
		eolUnix++;

	return string(buffer);
}


/*
template<typename T>
string ASStreamIterator<T>::nextLine()
{
    char *srcPtr;
    char *filterPtr;

    inStream->getline(buffer, 2047);
    srcPtr = filterPtr = buffer;

    while (*srcPtr != 0)
    {
        if (*srcPtr != '\r')
            *filterPtr++ = *srcPtr;
        srcPtr++;
    }
    *filterPtr = 0;

    return string(buffer);
}
*/

template<class ITER>
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

void manuallySetJavaStyle(ASFormatter &formatter)
{
	formatter.setJavaStyle();
	_modeManuallySet = true;
}

void manuallySetCStyle(ASFormatter &formatter)
{
	formatter.setCStyle();
	_modeManuallySet = true;
}


bool parseOption(ASFormatter &formatter, const string &arg, const string &errorInfo)
{
	if ( IS_OPTION(arg, "style=ansi") )
	{
		formatter.setBracketIndent(false);
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(BREAK_MODE);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=gnu") )
	{
		formatter.setBlockIndent(true);
		formatter.setSpaceIndentation(2);
		formatter.setBracketFormatMode(BREAK_MODE);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=java") )
	{
		manuallySetJavaStyle(formatter);
		formatter.setBracketIndent(false);
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(ATTACH_MODE);
		formatter.setSwitchIndent(false);
	}
	else if ( IS_OPTION(arg, "style=kr") )
	{
		//manuallySetCStyle(formatter);
		formatter.setBracketIndent(false);
		formatter.setSpaceIndentation(4);
		formatter.setBracketFormatMode(ATTACH_MODE);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTION(arg, "style=linux") )
	{
		formatter.setBracketIndent(false);
		formatter.setSpaceIndentation(8);
		formatter.setBracketFormatMode(BDAC_MODE);
		formatter.setClassIndent(false);
		formatter.setSwitchIndent(false);
		formatter.setNamespaceIndent(false);
	}
	else if ( IS_OPTIONS(arg, "c", "mode=c") )
	{
		manuallySetCStyle(formatter);
	}
	else if ( IS_OPTIONS(arg, "j", "mode=java") )
	{
		manuallySetJavaStyle(formatter);
	}
	else if ( isParamOption(arg, "t", "indent=tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "t", "indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			(*_err) << "Error in param: " << arg << endl;
		else
			formatter.setTabIndentation(spaceNum, false);
	}
	else if ( isParamOption(arg, "T", "force-indent=tab=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "T", "force-indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			(*_err) << "Error in param: " << arg << endl;
		else
			formatter.setTabIndentation(spaceNum, true);
	}
	else if ( IS_OPTION(arg, "indent=tab") )
	{
		formatter.setTabIndentation(4);
	}
	else if ( isParamOption(arg, "s", "indent=spaces=") )
	{
		int spaceNum = 4;
		string spaceNumParam = GET_PARAMS(arg, "s", "indent=spaces=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			(*_err) << "Error in param: " << arg << endl;
		else
			formatter.setSpaceIndentation(spaceNum);
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
			(*_err) << "Error in param: " << arg << endl;
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
			(*_err) << "Error in param: " << arg << endl;
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
	else if ( IS_OPTION(arg, "brackets=break-closing-headers") )
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
		formatter.setBracketFormatMode(BDAC_MODE);
	}
	else if ( IS_OPTIONS(arg, "O", "one-line=keep-blocks") )
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if ( IS_OPTIONS(arg, "o", "one-line=keep-statements") )
	{
		formatter.setSingleStatementsMode(false);
	}
	else if ( IS_OPTIONS(arg, "P", "pad=paren") )
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "d", "pad=paren-out") )
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "D", "pad=paren-in") )
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "U", "unpad=paren") )
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "p", "pad=oper") )
	{
		formatter.setOperatorPaddingMode(true);
	}
	else if ( IS_OPTIONS(arg, "E", "fill-empty-lines") )
	{
		formatter.setEmptyLineFill(true);
	}
	else if ( IS_OPTION(arg, "indent-preprocessor") )
	{
		formatter.setPreprocessorIndent(true);
	}
	else if ( IS_OPTIONS(arg, "V", "convert-tabs") )
	{
		formatter.setTabSpaceConversionMode(true);
	}
	else if ( IS_OPTION(arg, "break-blocks=all") )
	{
		formatter.setBreakBlocksMode(true);
		formatter.setBreakClosingHeaderBlocksMode(true);
	}
	else if ( IS_OPTION(arg, "break-blocks") )
	{
		formatter.setBreakBlocksMode(true);
	}
	else if ( IS_OPTION(arg, "break-elseifs") )
	{
		formatter.setBreakElseIfsMode(true);
	}
// Options used by GUI
#if defined(ASTYLE_GUI) || defined(ASTYLE_DLL)
	else
	{
		if (_err->str().length() == 0)
			(*_err) << errorInfo << endl;
		(*_err) << endl << arg;
		return false; // unknown option
	}
// Options used by console
#else
	else if ( isParamOption(arg, "suffix=") )
	{
		string suffixParam = GET_PARAM(arg, "suffix=");
		if (suffixParam.length() > 0)
			_suffix = suffixParam;
	}
	else if ( IS_OPTIONS(arg, "X", "errors-to-standard-output") )
	{
		_err = &cout;
	}
	else if ( IS_OPTIONS(arg, "v", "version") )
	{
		(*_err) << "Artistic Style " << _version << endl;
		exit(1);
	}
	else
	{
		(*_err) << errorInfo << arg << endl;
		return false; // unknown option
	}
#endif
// End of parseOption function
	return true; //o.k.
}


#if defined(ASTYLE_GUI) || defined(ASTYLE_DLL)
// *************************   GUI functions   *****************************************************
/*
 * IMPORTANT linker must have the command line parameter  /EXPORT:AStyleMain=_AStyleMain@16
 * For Dll only - "warning C4702: unreachable code" in the <vector> header
 *                is caused by using the Optimization options .
 *                /O2   Maximize speed
 *                /Og   Global optimizations
 *                /Ob2  Any suitable inline expansion
 *                This is a bug in the Microsoft compiler.  The program runs over twice as fast
 *                with the options set.  There hasn't been any problem so far.
*/
extern "C" EXPORT char* STDCALL
	AStyleMain(char*  lpTextIn,                // pointer to the data to be formatted
	           char*  lpOptions,               // pointer to AStyle options, separated by \n
	           fpError fpErrorHandler,         // pointer to error handler function
	           fpAlloc fpMemoryAlloc)          // pointer to memory allocation function
{
	if (fpErrorHandler == NULL)
		return NULL;

	if (lpTextIn == NULL)
	{
		fpErrorHandler(101, "No pointer to text input.\n- File will not be formatted.");
		return NULL;
	}
	if (lpOptions == NULL)
	{
		fpErrorHandler(102, "No pointer to AStyle options.\n- File will not be formatted.");
		return NULL;
	}
	if (fpMemoryAlloc == NULL)
	{
		fpErrorHandler(103, "No pointer to memory allocation function.\n- File will not be formatted.");
		return NULL;
	}

	ASFormatter formatter;

	string arg;
	vector<string> optionsVector;
	istringstream opt(lpOptions);
	_err = new stringstream;

	while (getline(opt, arg))
		optionsVector.push_back(arg);

	parseOptions(formatter,
	             optionsVector.begin(),
	             optionsVector.end(),
	             "Unknown Artistic Style options!");

	if (_err->str().length() > 0)
	{
		(*_err) << "\n\n- These options will not be processed.     ";
		fpErrorHandler(201, (char*) _err->str().c_str());
	}

	delete _err;
	_err = NULL;

	istringstream in(lpTextIn);
	ASStreamIterator<istringstream> streamIterator(&in);
	ostringstream out;
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		out << formatter.nextLine();
		if (streamIterator.eolWindows > streamIterator.eolUnix)
			out << '\r';
		out << endl;
	}

	unsigned long textSizeOut = out.str().length();
	char* textOut = fpMemoryAlloc(textSizeOut + 1);     // call memory allocation function
//    textOut = NULL;           // for testing
	if (textOut == NULL)
	{
		fpErrorHandler(110, "Allocation failure on Artistic Style output.\n- File will not be formatted.");
		return NULL;
	}

	strcpy(textOut, out.str().c_str());

	return textOut;
}

#else

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

			// break options on spaces, tabs or new-lines
			if (ch == ' ' || ch == '\t' || ch == '\n')
				break;
			else
				currentToken.append(1, ch);

		}
		while (in);

		if (currentToken.length() != 0)
			optionsVector.push_back(currentToken);
	}
}

bool stringEndsWith(const string &str, const string &suffix)
{
	int strIndex = str.length() - 1;
	int suffixIndex = suffix.length() - 1;

	while (strIndex >= 0 && suffixIndex >= 0)
	{
		if (tolower(str[strIndex]) != tolower(suffix[suffixIndex]))
			return false;

		--strIndex;
		--suffixIndex;
	}

	return true;
}


void error(const char *why, const char* what)
{
	(*_err) << why << ' ' << what << '\n' << endl;
	exit(1);
}


void printHelp()
{
	(*_err) << endl;
	(*_err) << "                            Artistic Style " << _version.c_str() << endl;
	(*_err) << "                              by Tal Davidson\n";
	(*_err) << "                               and Jim Pattee\n";
	(*_err) << endl;
	(*_err) << "Usage  :  astyle [options] Source1.cpp Source2.cpp  [...]\n";
	(*_err) << "          astyle [options] < Original > Beautified\n";
	(*_err) << endl;
	(*_err) << "When indenting a specific file, the resulting indented file RETAINS the\n";
	(*_err) << "original file-name. The original pre-indented file is renamed, with a\n";
	(*_err) << "suffix of \".orig\" added to the original filename.\n";
	(*_err) << endl;
	(*_err) << "By default, astyle is set up to indent C/C++/C# files, with 4 spaces per\n";
	(*_err) << "indent, a maximal indentation of 40 spaces inside continuous statements,\n";
	(*_err) << "and NO formatting.\n";
	(*_err) << endl;
	(*_err) << "Option's Format:\n";
	(*_err) << "----------------\n";
	(*_err) << "    Long options (starting with '--') must be written one at a time.\n";
	(*_err) << "    Short options (starting with '-') may be appended together.\n";
	(*_err) << "    Thus, -bps4 is the same as -b -p -s4.\n";
	(*_err) << endl;
	(*_err) << "Predefined Style Options:\n";
	(*_err) << "-------------------------\n";
	(*_err) << "    --style=ansi\n";
	(*_err) << "    ANSI style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=gnu\n";
	(*_err) << "    GNU style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=kr\n";
	(*_err) << "    Kernighan&Ritchie style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "    --style=linux\n";
	(*_err) << "    Linux mode (8 spaces per indent, break definition-block\n";
	(*_err) << "    brackets but attach command-block brackets).\n";
	(*_err) << endl;
	(*_err) << "    --style=java\n";
	(*_err) << "    Java mode, with standard java style formatting/indenting.\n";
	(*_err) << endl;
	(*_err) << "Tab and Bracket Options:\n";
	(*_err) << "------------------------\n";
	(*_err) << "    default indent option\n";
	(*_err) << "    If no indentation option is set,\n";
	(*_err) << "    the default option of 4 spaces will be used.\n";
	(*_err) << endl;
	(*_err) << "    --indent=spaces=#   OR   -s#\n";
	(*_err) << "    Indent using # spaces per indent. Not specifying #\n";
	(*_err) << "    will result in a default of 4 spaces per indent.\n";
	(*_err) << endl;
	(*_err) << "    --indent=tab   OR   --indent=tab=#   OR   -t   OR   -t#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Not specifying # will result\n";
	(*_err) << "    in a default assumption of 4 spaces per tab.\n";
	(*_err) << endl;
	(*_err) << "    --force-indent=tab=#   OR   -T#\n";
	(*_err) << "    Indent using tab characters, assuming that each\n";
	(*_err) << "    tab is # spaces long. Force tabs to be used in areas\n";
	(*_err) << "    Astyle would prefer to use spaces.\n";
	(*_err) << endl;
	(*_err) << "    default brackets option\n";
	(*_err) << "    If no brackets option is set,\n";
	(*_err) << "    the brackets will not be changed.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break   OR   -b\n";
	(*_err) << "    Break brackets from pre-block code (i.e. ANSI C/C++ style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=attach   OR   -a\n";
	(*_err) << "    Attach brackets to pre-block code (i.e. Java/K&R style).\n";
	(*_err) << endl;
	(*_err) << "    --brackets=linux   OR   -l\n";
	(*_err) << "    Break definition-block brackets and attach command-block\n";
	(*_err) << "    brackets.\n";
	(*_err) << endl;
	(*_err) << "    --brackets=break-closing-headers\n";
	(*_err) << "    Break brackets before closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << "    Should be appended to --brackets=attach or --brackets=linux.\n";
	(*_err) << endl;
	(*_err) << "Indentation options:\n";
	(*_err) << "--------------------\n";
	(*_err) << "    --indent-classes   OR   -C\n";
	(*_err) << "    Indent 'class' blocks, so that the inner 'public:',\n";
	(*_err) << "    'protected:' and 'private: headers are indented in\n";
	(*_err) << "    relation to the class block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-switches   OR   -S\n";
	(*_err) << "    Indent 'switch' blocks, so that the inner 'case XXX:'\n";
	(*_err) << "    headers are indented in relation to the switch block.\n";
	(*_err) << endl;
	(*_err) << "    --indent-cases   OR   -K\n";
	(*_err) << "    Indent case blocks from the 'case XXX:' headers.\n";
	(*_err) << "    Case statements not enclosed in blocks are NOT indented.\n";
	(*_err) << endl;
	(*_err) << "    --indent-brackets   OR   -B\n";
	(*_err) << "    Add extra indentation to '{' and '}' block brackets.\n";
	(*_err) << endl;
	(*_err) << "    --indent-blocks   OR   -G\n";
	(*_err) << "    Add extra indentation entire blocks (including brackets).\n";
	(*_err) << endl;
	(*_err) << "    --indent-namespaces   OR   -N\n";
	(*_err) << "    Indent the contents of namespace blocks.\n";
	(*_err) << endl;
	(*_err) << "    --indent-labels   OR   -L\n";
	(*_err) << "    Indent labels so that they appear one indent less than\n";
	(*_err) << "    the current indentation level, rather than being\n";
	(*_err) << "    flushed completely to the left (which is the default).\n";
	(*_err) << endl;
	(*_err) << "    --indent-preprocessor\n";
	(*_err) << "    Indent multi-line #define statements.\n";
	(*_err) << endl;
	(*_err) << "    --max-instatement-indent=#   OR   -M#\n";
	(*_err) << "    Indent a maximal # spaces in a continuous statement,\n";
	(*_err) << "    relative to the previous line.\n";
	(*_err) << endl;
	(*_err) << "    --min-conditional-indent=#   OR   -m#\n";
	(*_err) << "    Indent a minimal # spaces in a continuous conditional\n";
	(*_err) << "    belonging to a conditional header.\n";
	(*_err) << endl;
	(*_err) << "Formatting options:\n";
	(*_err) << "-------------------\n";
	(*_err) << "    --break-blocks\n";
	(*_err) << "    Insert empty lines around unrelated blocks, labels, classes, ...\n";
	(*_err) << endl;
	(*_err) << "    --break-blocks=all\n";
	(*_err) << "    Like --break-blocks, except also insert empty lines \n";
	(*_err) << "    around closing headers (e.g. 'else', 'catch', ...).\n";
	(*_err) << endl;
	(*_err) << "    --break-elseifs\n";
	(*_err) << "    Break 'else if()' statements into two different lines.\n";
	(*_err) << endl;
	(*_err) << "    --pad=oper   OR   -p\n";
	(*_err) << "    Insert space paddings around operators.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren   OR   -P\n";
	(*_err) << "    Insert space padding around parenthesis on both the outside\n";
	(*_err) << "    and the inside.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren-out   OR   -d\n";
	(*_err) << "    Insert space padding around parenthesis on the outside only.\n";
	(*_err) << endl;
	(*_err) << "    --pad=paren-in   OR   -D\n";
	(*_err) << "    Insert space padding around parenthesis on the inside only.\n";
	(*_err) << endl;
	(*_err) << "    --unpad=paren   OR   -U\n";
	(*_err) << "    Remove unnecessary space padding around parenthesis.  This\n";
	(*_err) << "    can be used in combination with the 'pad' options above.\n";
	(*_err) << endl;
	(*_err) << "    --one-line=keep-statements   OR   -o\n";
	(*_err) << "    Don't break lines containing multiple statements into\n";
	(*_err) << "    multiple single-statement lines.\n";
	(*_err) << endl;
	(*_err) << "    --one-line=keep-blocks   OR   -O\n";
	(*_err) << "    Don't break blocks residing completely on one line.\n";
	(*_err) << endl;
	(*_err) << "    --convert-tabs   OR   -V\n";
	(*_err) << "    Convert tabs to spaces.\n";
	(*_err) << endl;
	(*_err) << "    --fill-empty-lines   OR   -E\n";
	(*_err) << "    Fill empty lines with the white space of their\n";
	(*_err) << "    previous lines.\n";
	(*_err) << endl;
	(*_err) << "    --mode=c   OR   -c\n";
	(*_err) << "    Indent a C, C++ or C# source file (this is the default).\n";
	(*_err) << endl;
	(*_err) << "    --mode=java   OR   -j\n";
	(*_err) << "    Indent a Java(TM) source file.\n";
	(*_err) << endl;
	(*_err) << "Other options:\n";
	(*_err) << "--------------\n";
	(*_err) << "    --suffix=####\n";
	(*_err) << "    Append the suffix #### instead of '.orig' to original filename.\n";
	(*_err) << endl;
	(*_err) << "    --options=####\n";
	(*_err) << "    Specify an options file #### to read and use.\n";
	(*_err) << endl;
	(*_err) << "    --options=none\n";
	(*_err) << "    Disable the default options file.\n";
	(*_err) << "    Only the command-line parameters will be used.\n";
	(*_err) << endl;
	(*_err) << "    --errors-to-standard-output   OR   -X\n";
	(*_err) << "    Print errors and help information to standard-output rather than\n";
	(*_err) << "    to standard-error.\n";
	(*_err) << endl;
	(*_err) << "    --version   OR   -v\n";
	(*_err) << "    Print version number.\n";
	(*_err) << endl;
	(*_err) << "    --help   OR   -h   OR   -?\n";
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

int main(int argc, char *argv[])
{
	ASFormatter formatter;
	vector<string> fileNameVector;
	vector<string> optionsVector;
	string optionsFileName = "";
	string arg;
	bool ok = true;
	bool shouldPrintHelp = false;
	bool shouldParseOptionsFile = true;

	_err = &cerr;
	_suffix = ".orig";
	_modeManuallySet = false;

	(*_err) << "\nArtistic Style " << _version << endl;      // begin formatting
	// manage flags
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
		}
		else if ( IS_OPTION(arg, "-h")
		          || IS_OPTION(arg, "--help")
		          || IS_OPTION(arg, "-?") )
		{
			shouldPrintHelp = true;
		}
		else if (arg[0] == '-')
		{
			optionsVector.push_back(arg);
		}
		else // file-name
		{
			fileNameVector.push_back(arg);
		}
	}

	// parse options file
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
		{
			ifstream optionsIn(optionsFileName.c_str());
			if (optionsIn)
			{
				(*_err) << "Using default options file " << optionsFileName << endl;
				vector<string> fileOptionsVector;
				importOptions(optionsIn, fileOptionsVector);
				ok = parseOptions(formatter,
				                  fileOptionsVector.begin(),
				                  fileOptionsVector.end(),
				                  string("Unknown option in default options file: "));
			}

			optionsIn.close();
			if (!ok)
			{
				(*_err) << "For help on options, type 'astyle -h' " << endl;
			}
		}
	}

	// parse options from command line

	ok = parseOptions(formatter,
	                  optionsVector.begin(),
	                  optionsVector.end(),
	                  string("Unknown command line option: "));
	if (!ok)
	{
		(*_err) << "For help on options, type 'astyle -h' \n" << endl;
		exit(1);
	}

	if (shouldPrintHelp)
	{
		printHelp();
		exit(1);
	}

	// if no files have been given, use cin for input and cout for output
	if (fileNameVector.empty())
	{
		// display file formatting message
		(*_err) << "formatting cin\n" << endl;

		ASStreamIterator<istream> streamIterator(&cin);

		formatter.init(&streamIterator);

		while (formatter.hasMoreLines())
		{
			cout << formatter.nextLine();
			if (formatter.hasMoreLines())
				cout << endl;
		}
		cout.flush();
	}
	else
	{
		// indent the given files
		for (size_t i = 0; i < fileNameVector.size(); i++)
		{
			string originalFileName = fileNameVector[i];
			string inFileName = originalFileName + _suffix;

			remove(inFileName.c_str());

			// check if the file is present before rename
			ifstream inCheck(originalFileName.c_str());
			if (!inCheck)
				error("Could not open input file", originalFileName.c_str());
			else
				inCheck.close();

			if (rename(originalFileName.c_str(), inFileName.c_str()) < 0)
				error("Could not rename ", string(originalFileName + " to " + inFileName).c_str());

			ifstream in(inFileName.c_str());
			if (!in)
				error("Could not open input file", inFileName.c_str());

			ofstream out(originalFileName.c_str());
			if (!out)
				error("Could not open output file", originalFileName.c_str());

			// Unless a specific language mode has been, set the language mode
			// according to the file's suffix.
			if (!_modeManuallySet)
			{
				if (stringEndsWith(originalFileName, string(".java")))
				{
					formatter.setJavaStyle();
				}
				else
				{
					formatter.setCStyle();
				}
			}
			// display file formatting message
			(*_err) << "formatting " << originalFileName.c_str() << endl;

			ASStreamIterator<istream> streamIterator(&in);
			formatter.init(&streamIterator);

			while (formatter.hasMoreLines())
			{
				out << formatter.nextLine();
				if (formatter.hasMoreLines())
				{
					if (streamIterator.eolWindows > streamIterator.eolUnix)
						out << '\r';
					out << endl;
				}
			}
			out.flush();
			out.close();
			in.close();
		}
		(*_err) << endl;                    // all files formatted
	}
	return 0;
}

#endif
// *************************   end of console functions   *****************************************
