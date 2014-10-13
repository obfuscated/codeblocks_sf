/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

/**
 * Purpose: Simulate a CC parsing run.
 * <CALL_GRAPH>
 * CCTestApp():
 * - Creates the CCTestFrame
 *
 * -> CCTestFrame(): contains an instance of NativeParserTest class
 *   -> CCTestFrame::Start():
 *      - Reads all UI values into global vars (includes)
 *      - compiles initial global file queue
 *      - Creates global "Busy" dialog
 *      - Iterates over global file queue and calls NativeParserTest::ParseAndCodeCompletion(file)
 *      - destroys "Busy" dialog
 *
 * -> NativeParserTest(): contains a ParserBase instance, which mimic the NativeParser in C::B.
 *   -> NativeParserTest()::ParseAndCodeCompletion(): parse the file, run tests contains in the file
 *      - Determine whether the file should be read from hard disk or wxScintilla control
 *      - parse it by calling ParserBase::ReParse() or ParserBase::ParseBuffer()
 *      - prepares an expression string
 *      - Tests the expression matching algorithms by call NativeParserTest::TestExpression()
 *      - Prints results to UI
 *   -> NativeParserTest::Init() the macro replacements are setup
 *
 * -> ParserBase(): holds tree "TokenTree* m_pTokenTree;" and the include search paths
 *      - ParserCommon::FileType():
 *        - determines file type as source/header according extension
 *      - ParserBase::GetFullFileName():
 *        - uses includes provided to compute full file name
 *      - ParserBase::ParseFile():
 *        - Monitors the parsing of files to avoid re-parsing the same file
 *        - Appends new files to file global file queue
 *    -> ParserBase::Reparse() or ParserBase::ParseBuffer():
 *      - Creates and initializes FileLoader for provided file
 *      - Sets up ParserThreadOptions (like follow local/global includes etc...)
 *      - Creates a ParserBase instance "client"
 *      - Creates a new ParserThread
 *      - Calls ParserThread::Parse() and provides:
 *        ParserBase, file, ParserThreadOptions, TokenTree
 *      - Allows to access the results, like tokens tree
 *
 * -> ParserThread(): same as in Code::Blocks implementation
 *    -> ParserThread::Parse():
 *       - Calls ParserThread::InitTokenizer()
 *       - Reserves file for parsing in TokenTree
 *       - Calls ParserThread::DoParse() (if file is not flagged as assigned already)
 *       - Flags file as parsed.
 *    -> ParserThread::InitTokenizer():
 *       - Read the file or buffer and tokenises it into elements
 *    -> ParserThread::DoParse():
 *       - Recursive function that handles all the dirty stuff
 *       - Calls ParserCommon::FileType() in ParserThread::HandleIncludes()
 *         to parse additionally encountered files (#includes)
 *       - Calls ParserBase::ParseFile() in ParserThread::HandleIncludes()
 *         to parse additionally encountered files (#includes)
 *
 * </CALL_GRAPH>
 */

/**
 * In the CCTestFrame class, we have a member of NativeParserTest which is derived from
 * NativeParserBase class.
 * In the NativeParserTest class, we have a member of ParserBase.
 *
 * To parse a local file, we need the ParserBase::Reparse() function, which simply create a
 * ParserThread instance, and run ParserThread::Parse().
 *
 * To parse a buffer(the buffer is the content of wxScintilla edit control), we need ParseBuffer()
 * function from ParserBase class, which also run a ParserThread::Parse() there.
 *
 * To store the Tokens, we have a TokenTree member in ParserBase class.
 *
 * To handle #include directive in ParserThread::Parse() function, we need to use a function
 * ParserBase::GetFullFileName(). That's because ParserThread will generally call
 * m_Parent->GetFullFileName() to locate the file, and then m_Parent->ParseFile() to recursively
 * handling the included files.
 */

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//(*AppHeaders
#include <wx/image.h>
//*)

#include <wx/app.h>
#include <wx/arrstr.h>

#include "cctest_frame.h"

namespace CCTestAppGlobal
{
    // global variable to ease handling of include directories and header files
    wxArrayString s_includeDirs;
    wxArrayString s_fileQueue;
    wxArrayString s_filesParsed;
}// CCTestAppGlobal

class CCTestApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(CCTestApp)
DECLARE_APP(CCTestApp)

bool CCTestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    //*)

    CCTestFrame* frame = new CCTestFrame(_T(""));
    frame->Center();
    frame->Show();
    // run testing here
    frame->Start();

    return wxsOK;
}
