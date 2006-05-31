#include "../wxsheaders.h"
#include "wxslistbook.h"

#include <wx/splitter.h>
#include <wx/listbook.h>
#include "../properties/wxsenumproperty.h"
#include "../wxswidgetfactory.h"
#include "../resources/wxswindowres.h"
#include "wxslistbookparentqp.h"

WXS_ST_BEGIN(wxsListbookStyles)
    WXS_ST_CATEGORY("wxListbook")
    WXS_ST(wxLB_DEFAULT)
    WXS_ST(wxLB_TOP)
    WXS_ST(wxLB_LEFT)
    WXS_ST(wxLB_RIGHT)
    WXS_ST(wxLB_BOTTOM)
WXS_ST_END(wxsListbookStyles)

WXS_EV_BEGIN(wxsListbookEvents)
    WXS_EVI(EVT_LISTBOOK_PAGE_CHANGED,wxListbookEvent,PageChanged)
    WXS_EVI(EVT_LISTBOOK_PAGE_CHANGING,wxListbookEvent,PageChanging)
WXS_EV_END(wxsListbookEvents)

class wxsListbookPreview: public wxListbook
{
	public:
        wxsListbookPreview(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, wxsListbook* NB):
            wxListbook(parent,id,pos,size,style),
            Listbook(NB)
        {}

    private:

        wxsListbook* Listbook;
        void OnChanged(wxListbookEvent& event)
        {
//        	wxsWidget* NewSelection = Listbook->GetChild(event.GetSelection());
//        	if ( NewSelection != Listbook->CurrentSelection )
//        	{
//        		Listbook->CurrentSelection = NewSelection;
//                Listbook->PropertiesUpdated(false,false);
//        	}
        }

        DECLARE_EVENT_TABLE()
};

wxsListbook::wxsListbook(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,0),
    CurrentSelection(0)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
}

wxsListbook::~wxsListbook()
{
}

bool wxsListbook::CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer ) return false;
	if ( NewWidget->GetInfo().Spacer ) return false;
	return true;
}

int wxsListbook::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer )
	{
		wxMessageBox(_("Can not add sizer into Listbook.\nAdd panels first"));
		return -1;
	}

	if ( NewWidget->GetInfo().Spacer )
	{
		wxMessageBox(_("Spacer can be added to sizers only"));
		return -1;
	}

	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}

wxWindow* wxsListbook::MyCreatePreview(wxWindow* Parent)
{
	return new wxListbook(Parent,-1,GetPosition(),GetSize(),GetStyle());
}

void wxsListbook::MyFinalUpdatePreview(wxWindow* Preview)
{
	wxListbook* Wnd = (wxListbook*)Preview;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsListbookExtraParams* Params = GetExtraParams(i);
		Wnd->AddPage(Child->GetPreview(),Params->Label,Child==CurrentSelection);
	}
}

wxString wxsListbook::GetProducingCode(const wxsCodeParams& Params)
{
	return wxString::Format(
        _T("%s = new wxListbook(%s,%s,%s,%s,%s);"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        Params.Style.c_str());
}

wxString wxsListbook::GetFinalizingCode(const wxsCodeParams& Params)
{
	wxString Code;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsListbookExtraParams* ExParams = GetExtraParams(i);
		Code += wxString::Format(
            _T("%s->AddPage(%s,%s,%s);\n"),
                Params.VarName.c_str(),
                Child->BaseProperties.VarName.c_str(),
                wxsGetWxString(ExParams->Label).c_str(),
                ExParams->Selected ? _T("true") : _T("false"));
	}
	return Code;
}

bool wxsListbook::XmlLoadChild(TiXmlElement* Element)
{
	if ( strcmp(Element->Value(),"object") ) return true;

	bool Ret = true;
	TiXmlElement* RealObject = Element;

    const char* Class = Element->Attribute("class");
    if ( Class && !strcmp(Class,"listbookpage") )
    {
        RealObject = Element->FirstChildElement("object");
    }

	if ( !RealObject ) return false;
	if ( !wxsWidget::XmlLoadChild(RealObject) ) return false;
    int Index = GetChildCount() - 1;
    if ( Index < 0 ) return false;

    wxsListbookExtraParams* Params = GetExtraParams(Index);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Element);
    Params->Label = XmlGetVariable(_T("label"));
    Params->Selected = XmlGetInteger(_T("selected"),0) != 0;
    XmlAssignElement(Store);

    if (Index == 0 || Params->Selected) CurrentSelection = GetChild(Index);

    return Ret;
}

bool wxsListbook::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
	bool Ret = true;
	TiXmlElement* ListbookPage = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
	ListbookPage->SetAttribute("class","listbookpage");

    wxsListbookExtraParams* Params = GetExtraParams(ChildIndex);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(ListbookPage);
    XmlSetVariable(_T("label"),Params->Label);
    if ( Params->Selected ) XmlSetInteger(_T("selected"),1);
    XmlAssignElement(Store);

    return wxsWidget::XmlSaveChild(ChildIndex,ListbookPage) && Ret;
}

void wxsListbook::AddChildProperties(int ChildIndex)
{
	wxsWidget* Widget = GetChild(ChildIndex);
	wxsListbookExtraParams* Params = GetExtraParams(ChildIndex);
	if ( !Widget || !Params ) return;

    Widget->Properties.AddProperty(_("Listbook page:"),Params->Label,0);
    Widget->Properties.AddProperty(_(" Page selected:"),Params->Selected,1);
}

void wxsListbook::PreviewMouseEvent(wxMouseEvent& event)
{
	if ( GetPreview() && event.LeftDown() )
	{
	    wxListbook* Preview = (wxListbook*)GetPreview();
	    int Hit = Preview->HitTest(wxPoint(event.GetX(),event.GetY()));
        if ( Hit != wxNOT_FOUND )
        {
            CurrentSelection = GetChild(Hit);
            PropertiesChanged(false,false);
        }
	}
}

void wxsListbook::EnsurePreviewVisible(wxsWidget* Child)
{
	CurrentSelection = NULL;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		if ( GetChild(i) == Child )
		{
			CurrentSelection = Child;
			if ( GetPreview() )
			{
				((wxListbook*)GetPreview())->SetSelection(i);
			}
		}
	}

	wxsWidget::EnsurePreviewVisible(Child);
}

wxWindow* wxsListbook::BuildChildQuickPanel(wxWindow* Parent,int ChildPos)
{
    wxsWidget* Widget = GetChild(ChildPos);
    wxsListbookExtraParams* Params = GetExtraParams(ChildPos);
    if ( !Widget || !Params ) return NULL;
    return new wxsListbookParentQP(Parent,Widget,Params);
}

bool wxsListbook::ChildReallyVisible(wxsWidget* Child)
{
    return Child == CurrentSelection;
}
