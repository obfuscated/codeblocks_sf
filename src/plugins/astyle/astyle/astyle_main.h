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

#ifndef ASTYLE_MAIN_H
#define ASTYLE_MAIN_H

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#include "astyle.h"

#if defined(_MSC_VER) || defined(__DMC__)
#include <sys/utime.h>
#include <sys/stat.h>
#else
#include <utime.h>
#include <sys/stat.h>
#endif                         // end compiler checks

#ifdef ASTYLE_JNI
#include <jni.h>
#ifndef ASTYLE_LIB    // ASTYLE_LIB must be defined for ASTYLE_JNI
#define ASTYLE_LIB
#endif
#endif  //  ASTYLE_JNI

// for G++ implementation of string.compare:
#if defined(__GNUC__) && __GNUC__ < 3
#error - Use GNU C compiler release 3 or higher
#endif

// for namespace problem in version 5.0
#if defined(_MSC_VER) && _MSC_VER < 1200        // check for V6.0
#error - Use Microsoft compiler version 6 or higher
#endif

//using namespace astyle;

namespace astyle
{

// return values for ASConsole
enum processReturn { CONTINUE, END_SUCCESS, END_FAILURE };

//----------------------------------------------------------------------------
// ASStreamIterator class
// typename will be istringstream for GUI and istream otherwise
// ASSourceIterator is an abstract class defined in astyle.h
//----------------------------------------------------------------------------

template<typename T>
class ASStreamIterator : public ASSourceIterator
{
	public:
		bool checkForEmptyLine;

		// function declarations
		ASStreamIterator(T *in);
		virtual ~ASStreamIterator();
		string nextLine(bool emptyLineWasDeleted);
		string peekNextLine();
		void peekReset();
		void saveLastInputLine();

		// inline functions
		bool compareToInputBuffer(const string &nextLine) const { return nextLine == prevBuffer; }
		const char* getOutputEOL() const { return outputEOL; }
		bool hasMoreLines() const { return !inStream->eof(); }

	private:
		T * inStream;          // pointer to the input stream
		string buffer;         // current input line
		string prevBuffer;     // previous input line
		int eolWindows;        // number of Windows line endings (CRLF)
		int eolLinux;          // number of Linux line endings (LF)
		int eolMacOld;         // number of old Mac line endings (CR)
		int peekStart;         // starting position for peekNextLine()
		char outputEOL[4];     // output end of line char
		bool prevLineDeleted;  // the previous input line was deleted
};

//----------------------------------------------------------------------------
// ASConsole class for console build
//----------------------------------------------------------------------------

class ASConsole
{
	public:
		// command line options
		bool isRecursive;                   // recursive option
		string origSuffix;                  // suffix= option
		bool noBackup;                      // suffix=none option
		bool isVerbose;                     // verbose option
		bool isQuiet;                       // quiet option
		bool isFormattedOnly;               // formatted lines only option
		bool optionsFileRequired;           // options= option
		// other variables
		bool hasWildcard;                   // file name includes a wildcard
		size_t mainDirectoryLength;         // directory length to be excluded in displays
		int  filesFormatted;                // number of files formatted
		int  filesUnchanged;                // number of files unchanged
		int  linesOut;                      // number of output lines

		vector<string> excludeVector;       // exclude from wildcard hits
		vector<bool>   excludeHitsVector;   // exclude flags for eror reporting
		vector<string> fileNameVector;      // file paths and names from the command line
		vector<string> optionsVector;       // options from the command line
		vector<string> fileOptionsVector;   // options from the options file
		vector<string> fileName;            // files to be processed including path
		string optionsFileName;             // file path and name of the options file to use
		string targetDirectory;             // path to the directory being processed
		string targetFilename;              // file name being processed


	public:
		ASConsole() {
			// command line options
			isRecursive = false;
			origSuffix = ".orig";
			noBackup = false;
			isVerbose = false;
			isQuiet = false;
			isFormattedOnly = false;
			optionsFileRequired = false;
			// other variables
			hasWildcard = false;
			mainDirectoryLength = 0;
			optionsFileName = "";
			filesFormatted = 0;
			filesUnchanged = 0;
			linesOut = 0;
		}

		// functions
		void error(const char *why, const char* what) const;
		void formatCinToCout(ASFormatter& formatter) const;
		bool formatFile(const string &fileName, astyle::ASFormatter &formatter) const;
		void getFilePaths(string &filePath);
		processReturn processOptions(int argc, char** argv, ASFormatter &formatter);
		void standardizePath(string &path, bool removeBeginningSeparator=false) const;

	private:
		string getCurrentDirectory(const string &fileName) const;
		void getFileNames(const string &directory, const string &wildcard);
		bool isPathExclued(const string &subPath);
		void printHelp() const;
		void removeFile(const char* fileName, const char* errMsg) const;
		void renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const;
		bool stringEndsWith(const string &str, const string &suffix) const;
		void wait(int seconds) const;
		int  waitForRemove(const char* oldFileName) const;
		int  wildcmp(const char *wild, const char *data) const;
};


//----------------------------------------------------------------------------
// global function declarations
// used by both console and library builds
//----------------------------------------------------------------------------

void importOptions(istream &in, vector<string> &optionsVector);
void isOptionError(const string &arg, const string &errorInfo);
bool isParamOption(const string &arg, const char *option);
bool isParamOption(const string &arg, const char *option1, const char *option2);
bool parseOption(astyle::ASFormatter &formatter, const string &arg, const string &errorInfo);

template<typename ITER>
bool parseOptions(astyle::ASFormatter &formatter, const ITER &optionsBegin,
                  const ITER &optionsEnd, const string &errorInfo);

}   // end of namespace astyle

//----------------------------------------------------------------------------
// declarations for java native interface (JNI) build
// global because they are called externally and are NOT part of the namespace
//----------------------------------------------------------------------------

#ifdef ASTYLE_JNI
void  STDCALL javaErrorHandler(int errorNumber, char* errorMessage);
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded);
// the following function names are constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass);
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain
(JNIEnv* env, jobject obj, jstring textInJava, jstring optionsJava);
#endif //  ASTYLE_JNI


#endif // closes ASTYLE_MAIN_H
