#include "wxslistbook.h"

#include <wx/splitter.h>
#include <wx/listbook.h>
#include "../properties/wxsenumproperty.h"
#include "../wxswidgetfactory.h"

WXS_ST_BEGIN(wxsListbookStyles)
    WXS_ST_CATEGORY("wxListbook")
    WXS_ST(wxLI_HORIZONTAL)
    WXS_ST(wxLI_VERTICAL)
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
        	wxsWidget* NewSelection = Listbook->GetChild(event.GetSelection());
        	if ( NewSelection != Listbook->CurrentSelection )
        	{
        		Listbook->CurrentSelection = NewSelection;
                Listbook->PropertiesUpdated(false,false);
        	}
        }
        
        DECLARE_EVENT_TABLE()
};

wxsListbook::wxsListbook(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsContainer(Man,Res,true,0,propWidget),
    CurrentSelection(0)
{
}

wxsListbook::~wxsListbook()
{
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

wxString wxsListbook::GetProducingCode(wxsCodeParams& Params)
{
	const CodeDefines& CD = GetCodeDefines();
	return wxString::Format(
        _T("%s = new wxListbook(%s,%s,%s,%s,%s);"),
        BaseParams.VarName.c_str(),
        Params.ParentName.c_str(),
        BaseParams.IdName.c_str(),
        CD.Pos.c_str(),
        CD.Size.c_str(),
        CD.Style.c_str());
}

wxString wxsListbook::GetFinalizingCode(wxsCodeParams& Params)
{
	wxString Code;
	for ( int i=0; i<GetChildCount(); ++i )
	{
		wxsWidget* Child = GetChild(i);
		wxsListbookExtraParams* Params = GetExtraParams(i);
		Code += wxString::Format(
            _T("%s->AddPage(%s,%s,%s);\n"),
                BaseParams.VarName.c_str(),
                Child->GetBaseParams().VarName.c_str(),
                GetWxString(Params->Label).c_str(),
                Params->Selected ? _T("true") : _T("false"));
	}
	return Code;
}

wxString wxsListbook::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxListbook* %s;"),BaseParams.VarName.c_str());
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
	
    const char* Name = RealObject->Attribute("class");
    
    if ( !Name || !*Name ) return false;
    
    wxsWidget* Child = wxsGEN(wxString(Name,wxConvUTF8),GetResource());
    if ( !Child ) return false;
    
    if ( !Child->XmlLoad(RealObject) ) Ret = false;
    int Index = AddChild(Child);
    if ( Index < 0 ) 
    {
        delete Child;
        return false;
    }
    
    wxsListbookExtraParams* Params = GetExtraParams(Index);
    TiXmlElement* Store = XmlElem();
    XmlAssignElement(Element);
    Params->Label = XmlGetVariable(_T("label"));
    Params->Selected = XmlGetInteger(_T("selected"),0) != 0;
    XmlAssignElement(Store);
    
    if (Index == 0 || Params->Selected) CurrentSelection = Child;
    
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
	
    Widget->GetPropertiesObj().AddProperty(_("Listbook page:"),Params->Label,0);
    Widget->GetPropertiesObj().AddProperty(_(" Page selected:"),Params->Selected,1);
}

void wxsListbook::PreviewMouseEvent(wxMouseEvent& event)
{
// FIXME (SpOoN#1#): Couldn't get it working :/
	if ( GetPreview() ) GetPreview()->ProcessEvent(event);
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
