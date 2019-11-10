/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "compiler_defs.h"
#include <cbproject.h>
#include <projectbuildtarget.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(CompilerCommands);

CompilerQueue::CompilerQueue()
    : m_LastWasRun(false)
{
}

CompilerQueue::~CompilerQueue()
{
    Clear();
}

void CompilerQueue::Clear()
{
    m_Commands.DeleteContents(true);
    m_Commands.Clear();
    m_Commands.DeleteContents(false);
}

size_t CompilerQueue::GetCount() const
{
    return m_Commands.GetCount();
}

bool CompilerQueue::LastCommandWasRun() const
{
    return m_LastWasRun;
}

void CompilerQueue::Add(CompilerCommand* cmd)
{
    if (cmd)
    {
        if (cmd->dir.IsEmpty() && cmd->project)
            cmd->dir = cmd->project->GetExecutionDir();
        m_Commands.Append(cmd);
    }
}

void CompilerQueue::Add(CompilerQueue* queue)
{
    for (CompilerCommands::iterator it = queue->m_Commands.begin(); it != queue->m_Commands.end(); ++it)
    {
        if (*it)
            Add(new CompilerCommand(**it));
    }
}

CompilerCommand* CompilerQueue::Peek()
{
    if (m_Commands.empty())
        return nullptr;
    else
        return m_Commands.front();
}

CompilerCommand* CompilerQueue::Next()
{
    if (m_Commands.empty())
        return nullptr;
    CompilerCommand* cmd = m_Commands.front();
    m_Commands.pop_front();
    m_LastWasRun = cmd ? cmd->isRun : false;
    return cmd;
}
