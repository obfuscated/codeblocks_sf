
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <logmanager.h>

#include "NassiEditorPanel.h"
#include "NassiFileContent.h"


BEGIN_EVENT_TABLE(NassiEditorPanel,cbEditorPanel)

END_EVENT_TABLE()


NassiEditorPanel::EditorsSet NassiEditorPanel::m_AllEditors;

NassiEditorPanel::NassiEditorPanel( const wxString &fileName, const wxString &title ):
    cbEditorPanel( fileName, title, new NassiFileContent() ),
    m_view(new NassiView((NassiFileContent *)m_filecontent )),
    m_diagramwindow(0)
{
    //m_view = new NassiView((NassiFileContent *)m_filecontent);
    m_diagramwindow = m_view->CreateDiagramWindow(this);
    m_view->Update();

    wxBoxSizer *BoxSizer = new wxBoxSizer(wxVERTICAL);
    BoxSizer->Add((wxWindow *)m_diagramwindow,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer);
    BoxSizer->SetSizeHints(this);
    ((wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook())->Layout();

    m_AllEditors.insert( this );

    m_filecontent->AddObserver(this);
}
NassiEditorPanel::~NassiEditorPanel()
{
    m_filecontent->RemoveObserver(this);
    m_AllEditors.erase( this );

    if ( m_view ) delete m_view;
    // diagram window will be deleted by its parent
}
void NassiEditorPanel::Update(wxObject* /*hint*/)
{
    UpdateModified();
}
bool NassiEditorPanel::IsNassiEditor( EditorBase* editor )
{
    return m_AllEditors.find( editor ) != m_AllEditors.end();
}
void NassiEditorPanel::CloseAllNassiEditors()
{
    EditorsSet s = m_AllEditors;
    for ( EditorsSet::iterator i = s.begin(); i != s.end(); ++i )
    {
        EditorManager::Get()->QueryClose( *i );
        (*i)->Close();
    }

    assert( m_AllEditors.empty() );
}
bool NassiEditorPanel::IsDrawingSource()
{
    return m_view->IsDrawingSource();
}
bool NassiEditorPanel::IsDrawingComment()
{
    return m_view->IsDrawingComment();
}
void NassiEditorPanel::EnableDrawSource(bool en)
{
    m_view->EnableDrawSource(en);
}
void NassiEditorPanel::EnableDrawComment(bool en)
{
    m_view->EnableDrawComment(en);
}
void NassiEditorPanel::ChangeToolTo(NassiView::NassiTools tool)
{
    m_view->ChangeToolTo(tool);
}
void NassiEditorPanel::ToolSelect()
{
    m_view->ToolSelect();
}

bool NassiEditorPanel::CanZoomIn()
{
    return m_view->CanZoomIn();
}
bool NassiEditorPanel::CanZoomOut()
{
    return m_view->CanZoomOut();
}
void NassiEditorPanel::ZoomIn()
{
    m_view->ZoomIn();
}
void NassiEditorPanel::ZoomOut()
{
    m_view->ZoomOut();
}
void NassiEditorPanel::Cut()
{
    m_view->Cut();
}
void NassiEditorPanel::Copy()
{
    m_view->Copy();
}
void NassiEditorPanel::Paste()
{
    m_view->Paste();
}
void NassiEditorPanel::DeleteSelection()
{
    m_view->DeleteSelection();
}
bool NassiEditorPanel::CanPaste() const
{
    return m_view->CanPaste();
}
//bool NassiEditorPanel::CanCopy() const
//{
//    return m_view->CanCopy();
//}
//bool NassiEditorPanel::CanCut() const
//{
//    return m_view->CanCut();
//}
bool NassiEditorPanel::HasSelection() const
{
    return m_view->HasSelection();
}
bool NassiEditorPanel::IsReadOnly() const
{
    return m_filecontent->IsReadOnly();
}
bool NassiEditorPanel::CanSelectAll() const
{
    return m_view->CanSelectAll();
}
void NassiEditorPanel::SelectAll()
{
    m_view->SelectAll();
}

bool NassiEditorPanel::CanExport()
{
    NassiFileContent *nfc = (NassiFileContent *)m_filecontent;
    return m_view->HasSelectedBricks() || nfc->GetFirstBrick();
}
#ifdef USE_SVG
void NassiEditorPanel::ExportSVG()
{
    m_view->ExportSVG();
}
#endif
void NassiEditorPanel::ExportCSource()
{
    m_view->ExportCSource();
}
void NassiEditorPanel::ExportVHDLSource()
{
    m_view->ExportVHDLSource();
}
#if wxUSE_POSTSCRIPT
void NassiEditorPanel::ExportPS()
{
    m_view->ExportPS();
}
#endif
void NassiEditorPanel::ExportStrukTeX()
{
    m_view->ExportStrukTeX();
}
void NassiEditorPanel::ExportBitmap()
{
    m_view->ExportBitmap();
}

bool NassiEditorPanel::GetCSource(wxTextOutputStream &text_stream, wxUint32 n)
{
    return m_view->ExportCSource(text_stream,n);
}

