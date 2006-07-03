#include "../wxsheaders.h"
#include "wxsnotebook.h"

#include <wx/splitter.h>
#include "../properties/wxsenumproperty.h"
#include "../wxswidgetfactory.h"
#include "../resources/wxswindowres.h"
#include "wxsnotebookparentqp.h"

WXS_ST_BEGIN(wxsNotebookStyles)
    WXS_ST_CATEGORY("wxNotebook")
    WXS_ST(wxNB_DEFAULT)
    WXS_ST(wxNB_LEFT)
    WXS_ST(wxNB_RIGHT)
    WXS_ST(wxNB_TOP)
    WXS_ST(wxNB_BOTTOM)
    WXS_ST_MASK(wxNB_FIXEDWIDTH,wxsSFWin,0,true)
    WXS_ST_MASK(wxNB_MULTILINE,wxsSFWin,0,true)
    WXS_ST_MASK(wxNB_NOPAGETHEME,wxsSFWin,0,true)

// NOTE (cyberkoa##): wxNB_FLAT is in HELP (WinCE only) file but not in wxMSW's XRC
    WXS_ST_MASK(wxNB_FLAT,wxsSFWinCE,0,true)
WXS_ST_END(wxsNotebookStyles)

WXS_EV_BEGIN(wxsNotebookEvents)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxNotebookEvent,PageChanged)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxNotebookEvent,PageChanging)
WXS_EV_END(wxsNotebookEvents)

class wxsNotebookPreview: public wxNotebook
{
	public:
        wxsNotebookPreview(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, wxsNotebook* NB):
            wxNotebook(parent,id,pos,size,style),
            Notebook(NB)
        {}

    private:

        wxsNotebook* Notebook;
        void OnChanged(wxNotebookEvent& event)
        {
//        	wxsWidget* NewSelection = Notebook->GetChild(event.GetSelection());
//        	if ( NewSelection != Notebook->CurrentSelection )
//        	{
//        		Notebook->CurrentSelection = NewSelection;
//                Notebook->PropertiesUpdated(false,false);
//        	}
        }

        DECLARE_EVENT_TABLE()
};

wxsNotebook::wxsNotebook(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,0),
    CurrentSelection(0)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
}

wxsNotebook::~wxsNotebook()
{
}

bool wxsNotebook::CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer ) return false;
	if ( NewWidget->GetInfo().Spacer ) return false;
	return true;
}

int wxsNotebook::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer )
	{
//		wxMessageBox(_("Can not add sizer into Notebook.\nAdd panels first"));
//		return -1;

        // Small fix - we will add extra panel in order to add sizer
        CurrentSelection = wxsGEN(_T("wxPanel"),GetResource());
        int RetVal = wxsContainer::AddChild(CurrentSelection,InsertBeforeThis);
        CurrentSelection->AddChild(NewWidget,0);
        return RetVal;
	}

	if ( NewWidget->GetInfo().Spacer )
	{
		wxMessageBox(_("Spacer can be added to sizers only"));
		return -1;
	}

	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}

wxWindow* wxsNotebook::MyCreatePreview(wxWindow* Parent)
{
	wxNotebook* Notebook = new wxNotebook(Parent,-1,GetPosition(),GetSize(),GetStyle());
	if ( GetChildCount() == 0 )
	{
	    // Adding additional empty notebook
	    Notebook->AddPage(
            new wxPanel(Notebook,-1,wxDefaultPosition,wxSize(50,50)),
            _("No pages"));
	}
	return Notebook;
}

void wxsNotebook::MyFinalUpdatePreview(wxWindow* Preview)
{
	wxNotebook* Wnd = (wxNotebook*)Preview;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsNotebookExtraParams* Params = GetExtraParams(i);
		Wnd->AddPage(Child->GetPreview(),Params->Label,Child==CurrentSelection);
	}
}

wxString wxsNotebook::GetProducingCode(const wxsCodeParams& Params)
{
	return wxString::Format(
        _T("%s = new wxNotebook(%s,%s,%s,%s,%s,%s);"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        Params.Style.c_str(),
        Params.Name.c_str());
}

wxString wxsNotebook::GetFinalizingCode(const wxsCodeParams& Params)
{
	wxString Code;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsNotebookExtraParams* ExParams = GetExtraParams(i);
		Code += wxString::Format(
            _T("%s->AddPage(%s,%s,%s);\n"),
                Params.VarName.c_str(),
                Child->BaseProperties.VarName.c_str(),
                wxsGetWxString(ExParams->Label).c_str(),
                ExParams->Selected ? _T("true") : _T("false"));
	}
	return Code;
}

bool wxsNotebook::XmlLoadChild(TiXmlElement* Element)
{
	if ( strcmp(Element->Value(),"object") ) return true;

	bool Ret = true;
	TiXmlElement* RealObject = Element;

    const char* Class = Element->Attribute("class");
    if ( Class && !strcmp(Class,"notebookpage") )
    {
        RealObject = Element->FirstChildElement("object");
    }

	if ( !RealObject ) return false;
	if ( !wxsWidget::XmlLoadChild(RealObject) ) return false;
    int Index = GetChildCount() - 1;
    if ( Index < 0 ) return false;

    wxsNotebookExtraParams* Params = GetExtraParams(Index);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Element);
    Params->Label = XmlGetVariable(_T("label"));
    Params->Selected = XmlGetInteger(_T("selected"),0) != 0;
    XmlAssignElement(Store);

    if (Index == 0 || Params->Selected) CurrentSelection = GetChild(Index);

    return Ret;
}

bool wxsNotebook::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
	bool Ret = true;
	TiXmlElement* NotebookPage = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
	NotebookPage->SetAttribute("class","notebookpage");

    wxsNotebookExtraParams* Params = GetExtraParams(ChildIndex);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(NotebookPage);
    XmlSetVariable(_T("label"),Params->Label);
    if ( Params->Selected ) XmlSetInteger(_T("selected"),1);
    XmlAssignElement(Store);

    return wxsWidget::XmlSaveChild(ChildIndex,NotebookPage) && Ret;
}

void wxsNotebook::AddChildProperties(int ChildIndex)
{
	wxsWidget* Widget = GetChild(ChildIndex);
	wxsNotebookExtraParams* Params = GetExtraParams(ChildIndex);
	if ( !Widget || !Params ) return;

    Widget->Properties.AddProperty(_("Notebook page:"),Params->Label,0);
    Widget->Properties.AddProperty(_(" Page selected:"),Params->Selected,1);
}

void wxsNotebook::PreviewMouseEvent(wxMouseEvent& event)
{
	if ( GetPreview() && event.LeftDown() )
	{
	    wxNotebook* Preview = (wxNotebook*)GetPreview();
	    int Hit = Preview->HitTest(wxPoint(event.GetX(),event.GetY()));
        if ( Hit != wxNOT_FOUND )
        {
            CurrentSelection = GetChild(Hit);
            PropertiesChanged(false,false);
        }
	}
}

void wxsNotebook::EnsurePreviewVisible(wxsWidget* Child)
{
	CurrentSelection = NULL;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		if ( GetChild(i) == Child )
		{
			CurrentSelection = Child;
			if ( GetPreview() )
			{
				((wxNotebook*)GetPreview())->SetSelection(i);
			}
		}
	}

	wxsWidget::EnsurePreviewVisible(Child);
}

wxWindow* wxsNotebook::BuildChildQuickPanel(wxWindow* Parent,int ChildPos)
{
    wxsWidget* Widget = GetChild(ChildPos);
    wxsNotebookExtraParams* Params = GetExtraParams(ChildPos);
    if ( !Widget || !Params ) return NULL;
    return new wxsNotebookParentQP(Parent,Widget,Params);
}

bool wxsNotebook::ChildReallyVisible(wxsWidget* Child)
{
    return Child == CurrentSelection;
}
