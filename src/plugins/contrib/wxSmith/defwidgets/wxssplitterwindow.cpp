#include "../wxsheaders.h"
#include "wxssplitterwindow.h"

#include <wx/splitter.h>
#include "../properties/wxsenumproperty.h"

WXS_ST_BEGIN(wxsSplitterWindowStyles)
    WXS_ST_CATEGORY("wxSplitterWindow")
    WXS_ST(wxSP_3D)
    WXS_ST(wxSP_3DSASH)
    WXS_ST(wxSP_3DBORDER)
    WXS_ST(wxSP_BORDER)
    WXS_ST(wxSP_NOBORDER)
    WXS_ST(wxSP_NO_XP_THEME)
    WXS_ST(wxSP_PERMIT_UNSPLIT)
    WXS_ST(wxSP_LIVE_UPDATE)
WXS_ST_END(wxsSplitterWindowStyles)

WXS_EV_BEGIN(wxsSplitterWindowEvents)
    WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGING,wxSplitterEvent,SashPosChanging)
    WXS_EVI(EVT_SPLITTER_SASH_POS_CHANGED,wxSplitterEvent,SashPosChanged)
    WXS_EVI(EVT_SPLITTER_UNSPLIT,wxSplitterEvent,Unsplit)
    WXS_EVI(EVT_SPLITTER_DCLICK,wxSplitterEvent,DClick)
WXS_EV_END(wxsSplitterWindowEvents)


wxsSplitterWindow::wxsSplitterWindow(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,2),
    SashPos(0),
    MinSize(-1),
    Orientation(wxHORIZONTAL)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
}

wxsSplitterWindow::~wxsSplitterWindow()
{
}

int wxsSplitterWindow::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( GetChildCount() == 2 )
	{
		wxMessageBox(_("Splitter can have 2 children max"));
		return -1;
	}

	if ( NewWidget->GetInfo().Sizer )
	{
		wxMessageBox(_("Can not add sizer into Splitter.\nAdd panels first"));
		return -1;
	}

	if ( NewWidget->GetInfo().Spacer )
	{
		wxMessageBox(_("Spacer can be added to sizers only"));
		return -1;
	}

	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}

wxWindow* wxsSplitterWindow::MyCreatePreview(wxWindow* Parent)
{
	return new wxSplitterWindow(Parent,-1,GetPosition(),GetSize(),GetStyle());
}

void wxsSplitterWindow::MyFinalUpdatePreview(wxWindow* Preview)
{
	wxSplitterWindow* Wnd = (wxSplitterWindow*)Preview;
	if ( MinSize >= 0 ) Wnd->SetMinimumPaneSize(MinSize);
	if ( GetChildCount() == 0 ) return;
	if ( GetChildCount() == 1 )
	{
		Wnd->Initialize(GetChild(0)->GetPreview());
		return;
	}

	if ( Orientation==wxVERTICAL )
	{
		Wnd->SplitVertically(GetChild(0)->GetPreview(),GetChild(1)->GetPreview(),SashPos);
	}
	else
	{
		Wnd->SplitHorizontally(GetChild(0)->GetPreview(),GetChild(1)->GetPreview(),SashPos);
	}
}

wxString wxsSplitterWindow::GetProducingCode(wxsCodeParams& Params)
{
	const CodeDefines& CD = GetCodeDefines();
	return wxString::Format(
        _T("%s = new wxSplitterWindow(%s,%s,%s,%s,%s);"),
        GetBaseProperties().VarName.c_str(),
        Params.ParentName.c_str(),
        GetBaseProperties().IdName.c_str(),
        CD.Pos.c_str(),
        CD.Size.c_str(),
        CD.Style.c_str());
}

wxString wxsSplitterWindow::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	if ( MinSize >= 0 )
	{
		Code.Printf(_T("%s->SetMinimumPaneSize(%d);\n"),GetBaseProperties().VarName.c_str(),MinSize);
	}

	if ( GetChildCount() == 0 ) return Code;
	if ( GetChildCount() == 1 )
	{
		return Code + wxString::Format(_T("%s->Initialize(%s);\n"),
            GetBaseProperties().VarName.c_str(),
            GetChild(0)->GetBaseProperties().VarName.c_str());
	}

	if ( Orientation==wxVERTICAL )
	{
		return Code + wxString::Format(_T("%s->SplitVertically(%s,%s,%d);\n"),
            GetBaseProperties().VarName.c_str(),
            GetChild(0)->GetBaseProperties().VarName.c_str(),
            GetChild(1)->GetBaseProperties().VarName.c_str(),
            SashPos);
	}
    return Code + wxString::Format(_T("%s->SplitHorizontally(%s,%s,%d);\n"),
        GetBaseProperties().VarName.c_str(),
        GetChild(0)->GetBaseProperties().VarName.c_str(),
        GetChild(1)->GetBaseProperties().VarName.c_str(),
        SashPos);
}

wxString wxsSplitterWindow::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxSplitterWindow* %s;"),GetBaseProperties().VarName.c_str());
}

bool wxsSplitterWindow::MyXmlLoad()
{
	wxString Value = XmlGetVariable(_T("orientation"));
	Orientation = ( Value == _T("vertical") ) ? wxVERTICAL : wxHORIZONTAL;
	SashPos = XmlGetInteger(_T("sashpos"),0);
	MinSize = XmlGetInteger(_T("minsize"),-1);
    return true;
}

bool wxsSplitterWindow::MyXmlSave()
{
    if ( Orientation == wxVERTICAL ) XmlSetVariable(_T("orientation"),_T("vertical"));
    if ( SashPos != 0 ) XmlSetInteger(_T("sashpos"),SashPos);
    if ( MinSize >= 0 ) XmlSetInteger(_T("minsize"),MinSize);
    return true;
}


void wxsSplitterWindow::CreateObjectProperties()
{
	static const wxChar* Names[] = { _("Horizontal"), _("Vertical"), NULL };
	static long Values[] = { wxHORIZONTAL, wxVERTICAL };
    wxsWidget::CreateObjectProperties();
    PropertiesObject.AddProperty( _("Sash Position:"), SashPos,0);
    PropertiesObject.AddProperty( _("Min panel size:"),MinSize,1);
    PropertiesObject.AddProperty( _("Orientation:"), new wxsEnumProperty(&PropertiesObject,Orientation,Names,Values), 2);
}

