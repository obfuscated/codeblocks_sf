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
 * -> CCTestFrame(): will get a singleton CCTest instance pointer by calling CCTest::Get().
 *   - Provided an initial dummy file, can be set by the user later.
 *   -> CCTestFrame::Start():
 *      - Reads all UI values into global vars (includes, headers)
 *      - compiles initial global file queue
 *      - Creates global "Busy" dialog
 *      - Calls CCTest::Clear()
 *      - Iterates over global file queue and calls CCTest::Start(file)
 *      - Creates a local NativeParserTest instance, prepares an expression string
 *      - Tests the expression matching algorithms by call NativeParserTest::TestExpression()
 *      - Prints results to UI
 *      - destroys "Busy" dialog
 * -> CCTest(): holds dummy tree "TokenTree* m_pTokenTree;"
 *   -> CCTest::Start(file)
 *      - Note: In CCTest::Init() the macro replacements are setup
 *      - Creates and initializes FileLoader for provided file
 *      - Sets up ParserThreadOptions (like follow local/global includes etc...)
 *      - Creates a ParserBase instance "client"
 *      - Creates a new ParserThread
 *      - Calls ParserThread::Parse() and provides:
 *        ParserBase, file, ParserThreadOptions, TokenTree
 *      - Allows to access the results, like tokens tree
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
 *       - Calls ParserDummy::ParserCommon::FileType() in ParserThread::HandleIncludes()
 *         to parse additionally encountered files (#includes)
 *       - Calls ParserDummy::ParserBase::ParseFile() in ParserThread::HandleIncludes()
 *         to parse additionally encountered files (#includes)
 * -> ParserDummy(): it shares the header file "parser.h" with CodeCompletion plugin projects, but
 *        provides its own implementation to "Parser", namely:
 *      - ParserDummy::ParserCommon::FileType:
 *        - determines file type as source/header according extension
 *      - ParserDummy::ParserBase::GetFullFileName:
 *        - uses includes provided to compute full file name
 *      - ParserDummy::ParserBase::ParseFile():
 *        - Monitors the parsing of files to avoid re-parsing the same file
 *        - Appends new files to file global file queue
 * </CALL_GRAPH>
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

    wxLog::SetActiveTarget(new wxLogStderr());

    CCTestFrame* frame = new CCTestFrame(_T("cctest/test.h"));
    frame->Center();
    frame->Show();
    frame->Start();

    return wxsOK;
}
