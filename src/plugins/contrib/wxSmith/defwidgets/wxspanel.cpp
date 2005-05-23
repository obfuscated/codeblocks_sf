#include "wxspanel.h"

#include "wxsstdmanager.h"
#include <wx/panel.h>


wxsPanel::wxsPanel(wxsWidgetManager* Manager): wxsContainer(Manager)
{
}

wxsPanel::~wxsPanel()
{
}

const wxsWidgetInfo& wxsPanel::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsPanelId); 
}
        
const char* wxsPanel::GetProducingCode(wxsCodeParams& Params)
{
    static wxString Result;
    Result.Format("%s = new wxPanel(%s,-1);",BaseParams.VarName.c_str(),Params.ParentName);
    return Result.c_str();
}
        
wxWindow* wxsPanel::MyCreatePreview(wxWindow* Parent)
{
    return new wxPanel(Parent,-1);
}
