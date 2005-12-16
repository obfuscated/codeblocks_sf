#include "../wxsheaders.h"
#include "wxshtmlwindow.h"

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"
#include <wx/html/htmlwin.h>
#include <wx/filesys.h>

WXS_ST_BEGIN(wxsHtmlWindowStyles)
    WXS_ST_CATEGORY("wxHtmlWindow")
    WXS_ST(wxHW_SCROLLBAR_NEVER)
    WXS_ST(wxHW_SCROLLBAR_AUTO)
    WXS_ST(wxHW_NO_SELECTION)
WXS_ST_END(wxsHtmlWindowStyles)

WXS_EV_BEGIN(wxsHtmlWindowEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsHtmlWindowEvents)

wxsDWDefineBegin(wxsHtmlWindowBase,wxHtmlWindow,
    WXS_THIS = new wxHtmlWindow(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE);
    )
    wxsDWDefInt(borders,"Borders:",0);
    wxsDWDefStr(url,"Url:","");
    wxsDWDefLongStr(htmlcode,"Html Code:","");
wxsDWDefineEnd()

wxWindow* wxsHtmlWindow::MyCreatePreview(wxWindow* Parent)
{
    wxHtmlWindow* Wnd = new wxHtmlWindow(Parent,-1L,GetPosition(),GetSize(),GetStyle());
    if ( borders )
    {
        Wnd->SetBorders(borders);
    }
    if ( !url.empty() )
    {
        Wnd->LoadPage(url);
    }
    else if ( !htmlcode.empty() )
    {
        Wnd->SetPage(htmlcode);
    }
    
    return Wnd;
}

wxString wxsHtmlWindow::GetProducingCode(wxsCodeParams& Params)
{
    wxString Code;
    const CodeDefines& CDefs = GetCodeDefines();
    
    Code.Printf(_T("%s = new wxHtmlWindow(%s,%s,%s,%s,%s);\n"),
        GetBaseProperties().VarName.c_str(),
        Params.ParentName.c_str(),
        GetBaseProperties().IdName.c_str(),
        CDefs.Pos.c_str(),
        CDefs.Size.c_str(),
        CDefs.Style.c_str());
        
    if ( borders )
    {
        Code << wxString::Format(_T("%s->SetBorders(%d);\n"),
                GetBaseProperties().VarName.c_str(),
                borders);
    }
    if ( !url.empty() )
    {
        Code << wxString::Format(_T("%s->LoadPage(%s);\n"),
                GetBaseProperties().VarName.c_str(),
                GetWxString(url).c_str());
    }
    else if (!htmlcode.empty())
    {
        Code << wxString::Format(_T("%s->SetPage(%s);\n"),
                GetBaseProperties().VarName.c_str(),
                GetWxString(htmlcode).c_str());
    }
    
    Code << CDefs.InitCode;
    
    return Code;
}
