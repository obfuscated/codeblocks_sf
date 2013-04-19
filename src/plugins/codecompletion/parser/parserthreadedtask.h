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

class ParserThreadedTask : public cbThreadedTask
{
public:
    ParserThreadedTask(Parser* parser, wxMutex& parserCS);
    virtual int Execute();

private:
    Parser*  m_Parser;
    wxMutex& m_ParserMutex;
};

class MarkFileAsLocalThreadedTask : public cbThreadedTask
{
public:
    MarkFileAsLocalThreadedTask(Parser* parser, cbProject* project);
    virtual int Execute();

private:
    Parser*    m_Parser;
    cbProject* m_Project;
};

#endif // PARSERTHREADEDTASK_H
