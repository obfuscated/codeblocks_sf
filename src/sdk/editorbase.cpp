#include "editorbase.h"
#include "manager.h"
#include "editormanager.h"

EditorBase::EditorBase(wxMDIParentFrame* parent, const wxString& title)
    : wxMDIChildFrame(parent, -1, title),
    m_IsBuiltinEditor(false),
    m_pParent(parent),
    m_WinTitle(title)
{
// TODO (mandrav#1#): Respect relevant setting in environment options
//    Maximize(true);
    Manager::Get()->GetEditorManager()->AddCustomEditor(this);
}

EditorBase::~EditorBase()
{
    if (Manager::Get()->GetEditorManager()) // sanity check
        Manager::Get()->GetEditorManager()->RemoveCustomEditor(this);
}

const wxString& EditorBase::GetTitle()
{
    return m_WinTitle;
}

void EditorBase::SetTitle(const wxString& newTitle)
{
    m_WinTitle = newTitle;
}

wxMDIParentFrame* EditorBase::GetParent()
{
    return m_pParent;
}

bool EditorBase::Close()
{
    Destroy();
    return true;
}

bool EditorBase::IsBuiltinEditor()
{
    return m_IsBuiltinEditor;
}
