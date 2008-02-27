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
            cmd->dir = cmd->project->GetBasePath();
        m_Commands.Append(cmd);
    }
}

void CompilerQueue::Add(CompilerQueue* queue)
{
    wxCompilerCommandsNode* node = queue->m_Commands.GetFirst();
    while (node)
    {
        if (node->GetData())
            Add(new CompilerCommand(*(node->GetData())));
        node = node->GetNext();
    }
}

CompilerCommand* CompilerQueue::Peek()
{
    wxCompilerCommandsNode* node = m_Commands.GetFirst();
    if (!node)
        return 0;
    return node->GetData();
}

CompilerCommand* CompilerQueue::Next()
{
    wxCompilerCommandsNode* node = m_Commands.GetFirst();
    if (!node)
        return 0;
    CompilerCommand* cmd = node->GetData();
    m_Commands.Erase(node);
    m_LastWasRun = cmd ? cmd->isRun : false;
    return cmd;
}
