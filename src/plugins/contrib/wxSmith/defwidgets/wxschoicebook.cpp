#include "../wxsheaders.h"
#include "wxschoicebook.h"

#include <wx/splitter.h>
#include <wx/choicebk.h>
#include "../properties/wxsenumproperty.h"
#include "../wxswidgetfactory.h"
#include "../resources/wxswindowres.h"
#include "wxschoicebookparentqp.h"

WXS_ST_BEGIN(wxsChoicebookStyles)
    WXS_ST_CATEGORY("wxChoicebook")
    WXS_ST(wxCHB_DEFAULT)
    WXS_ST(wxCHB_LEFT)
    WXS_ST(wxCHB_RIGHT)
    WXS_ST(wxCHB_TOP)
    WXS_ST(wxCHB_BOTTOM)
WXS_ST_END(wxsChoicebookStyles)

WXS_EV_BEGIN(wxsChoicebookEvents)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGED,wxChoicebookEvent,PageChanged)
    WXS_EVI(EVT_NOTEBOOK_PAGE_CHANGING,wxChoicebookEvent,PageChanging)
WXS_EV_END(wxsChoicebookEvents)

class WXSCLASS wxsChoicebookPreview: public wxChoicebook
{
	public:
        wxsChoicebookPreview(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, wxsChoicebook* NB):
            wxChoicebook(parent,id,pos,size,style),
            Choicebook(NB)
        {}

    private:

        wxsChoicebook* Choicebook;
        void OnChanged(wxChoicebookEvent& event)
        {
//        	wxsWidget* NewSelection = Choicebook->GetChild(event.GetSelection());
//        	if ( NewSelection != Choicebook->CurrentSelection )
//        	{
//        		Choicebook->CurrentSelection = NewSelection;
//                Choicebook->PropertiesUpdated(false,false);
//        	}
        }

        DECLARE_EVENT_TABLE()
};

wxsChoicebook::wxsChoicebook(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,0),
    CurrentSelection(0)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
}

wxsChoicebook::~wxsChoicebook()
{
}

bool wxsChoicebook::CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer ) return false;
	if ( NewWidget->GetInfo().Spacer ) return false;
	return true;
}

int wxsChoicebook::AddChild(wxsWidget* NewWidget,int InsertBeforeThis)
{
	if ( NewWidget->GetInfo().Sizer )
	{
		wxMessageBox(_("Can not add sizer into Choicebook.\nAdd panels first"));
		return -1;
	}

	if ( NewWidget->GetInfo().Spacer )
	{
		wxMessageBox(_("Spacer can be added to sizers only"));
		return -1;
	}

	return wxsContainer::AddChild(NewWidget,InsertBeforeThis);
}

wxWindow* wxsChoicebook::MyCreatePreview(wxWindow* Parent)
{
	return new wxChoicebook(Parent,-1,GetPosition(),GetSize(),GetStyle());
}

void wxsChoicebook::MyFinalUpdatePreview(wxWindow* Preview)
{
	wxChoicebook* Wnd = (wxChoicebook*)Preview;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsChoicebookExtraParams* Params = GetExtraParams(i);
		Wnd->AddPage(Child->GetPreview(),Params->Label,Child==CurrentSelection);
	}
}

wxString wxsChoicebook::GetProducingCode(wxsCodeParams& Params)
{
	const CodeDefines& CD = GetCodeDefines();
	return wxString::Format(
        _T("%s = new wxChoicebook(%s,%s,%s,%s,%s);"),
        GetBaseProperties().VarName.c_str(),
        Params.ParentName.c_str(),
        GetBaseProperties().IdName.c_str(),
        CD.Pos.c_str(),
        CD.Size.c_str(),
        CD.Style.c_str());
}

wxString wxsChoicebook::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsChoicebookExtraParams* Params = GetExtraParams(i);
		Code += wxString::Format(
            _T("%s->AddPage(%s,%s,%s);\n"),
                GetBaseProperties().VarName.c_str(),
                Child->GetBaseProperties().VarName.c_str(),
                GetWxString(Params->Label).c_str(),
                Params->Selected ? _T("true") : _T("false"));
	}
	return Code;
}

wxString wxsChoicebook::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxChoicebook* %s;"),GetBaseProperties().VarName.c_str());
}

bool wxsChoicebook::XmlLoadChild(TiXmlElement* Element)
{
	if ( strcmp(Element->Value(),"object") ) return true;

	bool Ret = true;
	TiXmlElement* RealObject = Element;

    const char* Class = Element->Attribute("class");
    if ( Class && !strcmp(Class,"choicebookpage") )
    {
        RealObject = Element->FirstChildElement("object");
    }

	if ( !RealObject ) return false;

    const char* Name = RealObject->Attribute("class");

    if ( !Name || !*Name ) return false;

    wxsWidget* Child = wxsGEN(_U(Name),GetResource());
    if ( !Child ) return false;

    if ( !Child->XmlLoad(RealObject) ) Ret = false;
    int Index = AddChild(Child);
    if ( Index < 0 )
    {
        delete Child;
        return false;
    }

    wxsChoicebookExtraParams* Params = GetExtraParams(Index);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Element);
    Params->Label = XmlGetVariable(_T("label"));
    Params->Selected = XmlGetInteger(_T("selected"),0) != 0;
    XmlAssignElement(Store);

    if (Index == 0 || Params->Selected) CurrentSelection = Child;

    return Ret;
}

bool wxsChoicebook::XmlSaveChild(int ChildIndex,TiXmlElement* AddHere)
{
	bool Ret = true;
	TiXmlElement* ChoicebookPage = AddHere->InsertEndChild(TiXmlElement("object"))->ToElement();
	ChoicebookPage->SetAttribute("class","choicebookpage");

    wxsChoicebookExtraParams* Params = GetExtraParams(ChildIndex);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(ChoicebookPage);
    XmlSetVariable(_T("label"),Params->Label);
    if ( Params->Selected ) XmlSetInteger(_T("selected"),1);
    XmlAssignElement(Store);

    return wxsWidget::XmlSaveChild(ChildIndex,ChoicebookPage) && Ret;
}

void wxsChoicebook::AddChildProperties(int ChildIndex)
{
	wxsWidget* Widget = GetChild(ChildIndex);
	wxsChoicebookExtraParams* Params = GetExtraParams(ChildIndex);
	if ( !Widget || !Params ) return;

    Widget->GetPropertiesObj().AddProperty(_("Choicebook page:"),Params->Label,0);
    Widget->GetPropertiesObj().AddProperty(_(" Page selected:"),Params->Selected,1);
}

void wxsChoicebook::PreviewMouseEvent(wxMouseEvent& event)
{
	if ( GetPreview() && event.LeftDown() )
	{
	    wxChoicebook* Preview = (wxChoicebook*)GetPreview();
	    int Hit = Preview->HitTest(wxPoint(event.GetX(),event.GetY()));
        if ( Hit != wxNOT_FOUND )
        {
            CurrentSelection = GetChild(Hit);
            PropertiesUpdated(false,false);
        }
	}
}

void wxsChoicebook::EnsurePreviewVisible(wxsWidget* Child)
{
	CurrentSelection = NULL;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		if ( GetChild(i) == Child )
		{
			CurrentSelection = Child;
			if ( GetPreview() )
			{
				((wxChoicebook*)GetPreview())->SetSelection(i);
			}
		}
	}

	wxsWidget::EnsurePreviewVisible(Child);
}

wxWindow* wxsChoicebook::BuildChildQuickPanel(wxWindow* Parent,int ChildPos)
{
    wxsWidget* Widget = GetChild(ChildPos);
    wxsChoicebookExtraParams* Params = GetExtraParams(ChildPos);
    if ( !Widget || !Params ) return NULL;
    return new wxsChoicebookParentQP(Parent,Widget,Params);
}
