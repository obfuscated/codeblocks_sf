/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTHREADEDTASK_H
#define PARSERTHREADEDTASK_H

#include <wx/thread.h>

#include <cbthreadpool.h>

class Parser;
class cbProject;

/** a cbThreadedTask can be executed in cbThreadPool to do a lot of parsing jobs in
 * the begining stage of the batch parsing. Here, the files and the macro definition
 * are already prepared, so we put them in the thread pool
 */
class ParserThreadedTask : public cbThreadedTask
{
public:
    ParserThreadedTask(Parser* parser, wxMutex& parserCS);

    // this function really do the following jobs:
    // run sequence parsing jobs in a single thread, those include
    // 1, parsing predefined macro buffers
    // 2, parsing project files(mainly the implementation source files)
    virtual int Execute();

private:
    Parser*  m_Parser; /// a Parser object which contain TokenTree
    wxMutex& m_ParserMutex; /// mutex to access the Parser object
};

/** mark all the tokens belong to the project as "local"
 * E.g. If you have a.cpp in cbp, then all the tokens belong to a.cpp are marked as local
 * but not system headers like Windows.h, those tokens do not belong to the project files, so it
 * is not "local".
 */
class MarkFileAsLocalThreadedTask : public cbThreadedTask
{
public:
    MarkFileAsLocalThreadedTask(Parser* parser, cbProject* project);

    virtual int Execute();

private:
    Parser*    m_Parser;  /// a Parser object which contain TokenTree
    cbProject* m_Project; /// mutex to access the Parser object
};

#endif // PARSERTHREADEDTASK_H
