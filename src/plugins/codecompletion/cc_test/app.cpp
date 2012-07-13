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
 * ParserTestApp():
 * - Creates the Frame
 * -> Frame(): holds dummy parser "ParserTest m_ParserTest;"
 *   - Provided an initial dummy file, can be set by the user later.
 *   -> Frame::Start():
 *      - Reads all UI values into global vars (includes, headers)
 *      - compiles initial global file queue
 *      - Creates global "Busy" dialog
 *      - Calls ParserTest::Clear()
 *      - Iterates over global file queue and calls ParserTest::Start(file)
 *      - Prints results to UI
 *      - destroys "Busy" dialog
 * -> ParserTest(): holds dummy tree "TokensTree* m_pTokensTree;"
 *   -> ParserTest::Start(file)
 *      - Note: In ParserTest::ParserTest() the macro replacements are setup
 *      - Creates and initialises FileLoader for provided file
 *      - Sets up ParserThreadOptions (like follow local/global includes etc...)
 *      - Creates a ParserBase instance "client"
 *      - Creates a new ParserThread
 *      - Calls ParserThread::Parse() and provides:
 *        ParserBase, file, ParserThreadOptions, TokensTree
 *      - Allows to access the results, like tokens tree
 * -> ParserThread(): same as in Code::Blocks implementation
 *    -> ParserThread::Parse():
 *       - Calls ParserThread::InitTokenizer()
 *       - Reserves file for parsing in TokensTree
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
 * -> ParserDummy(): provides the implementation to "Parser", namely:
 *      - ParserDummy::ParserCommon::FileType:
 *        - determines file type as source/header according extension
 *      - ParserDummy::ParserBase::GetFullFileName:
 *        - uses includes provided to compute full file name
 *      - ParserDummy::ParserBase::ParseFile():
 *        - Monitors the parsing of files to avoid re-parsing the same file
 *        - Appends new files to file global file queue
 * </CALL_GRAPH>
 */

#include <sdk.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/app.h>
#endif

//(*AppHeaders
#include <wx/image.h>
//*)

#include <wx/arrstr.h>

#include "frame.h"

namespace CCTestAppGlobal
{
    // global variable to ease handling of include directories and header files
    wxArrayString s_includeDirs;
    wxArrayString s_fileQueue;
    wxArrayString s_filesParsed;
}// CCTestAppGlobal

bool s_DebugSmartSense = true;

class ParserTestApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(ParserTestApp)
DECLARE_APP(ParserTestApp)

bool ParserTestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    //*)

    wxLog::SetActiveTarget(new wxLogStderr());

    Frame* frame = new Frame(_T("test.h"));
    frame->Center();
    frame->Show();
    frame->Start();

    return wxsOK;
}
