#include <wx/notebook.h>
#include "editorbase.h"
#include "manager.h"
#include "editormanager.h"

EditorBase::EditorBase(wxWindow* parent, const wxString& title)
    : wxPanel(parent, -1),
    m_IsBuiltinEditor(false),
    m_WinTitle(title)
{
    Manager::Get()->GetEditorManager()->AddCustomEditor(this);
    SetTitle(title);
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
    int mypage = Manager::Get()->GetEditorManager()->FindPageFromEditor(this);
    if (mypage != -1)
        Manager::Get()->GetEditorManager()->GetNotebook()->SetPageText(mypage, newTitle);
}

void EditorBase::Activate()
{
    Manager::Get()->GetEditorManager()->SetActiveEditor(this);
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
