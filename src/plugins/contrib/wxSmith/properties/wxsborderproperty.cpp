#include "../wxsheaders.h"
#include "wxsborderproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../wxsdefsizer.h"

#ifdef __NO_PROPGRGID

    class WXSCLASS wxsBorderPropertyWindow: public wxPanel
    {
        public:
            wxsBorderPropertyWindow(wxWindow* Parent,wxsBorderProperty* Object);
            virtual ~wxsBorderPropertyWindow();

            void UpdateData();

        private:

            void OnButtonChanged(wxCommandEvent& event);

            //wxToggleButton
            wxCheckBox
                *Left, *Right, *Top, *Bottom;

            wxsBorderProperty* Object;
            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsBorderPropertyWindow,wxPanel)
        EVT_TOGGLEBUTTON(wxID_ANY,wxsBorderPropertyWindow::OnButtonChanged)
        EVT_CHECKBOX(wxID_ANY,wxsBorderPropertyWindow::OnButtonChanged)
    END_EVENT_TABLE()

    wxsBorderPropertyWindow::wxsBorderPropertyWindow(wxWindow* Parent,wxsBorderProperty* _Object):
        wxPanel(Parent,-1),
        Object(_Object)
    {
        Left = new wxCheckBox(this,-1,_T(""));
        Right = new wxCheckBox(this,-1,_T(""));
        Top = new wxCheckBox(this,-1,_T(""));
        Bottom = new wxCheckBox(this,-1,_T(""));

        wxFlexGridSizer* Sizer = new wxFlexGridSizer(3,1,1);

        Sizer->Add(1,1);
        Sizer->Add(Top);
        Sizer->Add(1,1);
        Sizer->Add(Left);
        Sizer->Add(1,1);
        Sizer->Add(Right);
        Sizer->Add(1,1);
        Sizer->Add(Bottom);
        Sizer->Add(1,1);

        SetSizer(Sizer);
        Sizer->SetSizeHints(this);
    }

    wxsBorderPropertyWindow::~wxsBorderPropertyWindow()
    {}

    void wxsBorderPropertyWindow::OnButtonChanged(wxCommandEvent& event)
    {
        int NewFlags =
            ( Left->GetValue()   ? wxsSizerExtraParams::Left   :  wxsSizerExtraParams::None ) |
            ( Right->GetValue()  ? wxsSizerExtraParams::Right  :  wxsSizerExtraParams::None ) |
            ( Top->GetValue()    ? wxsSizerExtraParams::Top    :  wxsSizerExtraParams::None ) |
            ( Bottom->GetValue() ? wxsSizerExtraParams::Bottom :  wxsSizerExtraParams::None );

        assert(Object != NULL);

        Object->BorderFlags = NewFlags;
        Object->ValueChanged(true);
    }

    void wxsBorderPropertyWindow::UpdateData()
    {
        assert ( Object != NULL );
        int Flags = Object->BorderFlags;

        Left->SetValue( (Flags&wxsSizerExtraParams::Left) != 0 );
        Right->SetValue( (Flags&wxsSizerExtraParams::Right) != 0 );
        Top->SetValue( (Flags&wxsSizerExtraParams::Top) != 0 );
        Bottom->SetValue( (Flags&wxsSizerExtraParams::Bottom) != 0 );

    }

#endif

wxsBorderProperty::wxsBorderProperty(wxsProperties* Properties,int& Flag):
    wxsProperty(Properties),
    BorderFlags(Flag),
    #ifdef __NO_PROPGRGID
        Window(NULL)
    #else
        PGId(0)
    #endif
{
	//ctor
}

wxsBorderProperty::~wxsBorderProperty()
{
	//dtor
}

const wxString& wxsBorderProperty::GetTypeName()
{
    static wxString Type(_T("Widget Border"));
    return Type;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxsBorderProperty::BuildEditWindow(wxWindow* Parent)
    {
        return Window = new wxsBorderPropertyWindow(Parent,this);
    }

    void wxsBorderProperty::UpdateEditWindow()
    {
        if ( Window )
        {
            Window->UpdateData();
        }
    }

#else

    void wxsBorderProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	static const wxChar* Borders[] =
    	{
    		_("Left"),
    		_("Right"),
    		_("Top"),
    		_("Bottom"),
    		NULL
    	};

    	static long Values[] =
    	{
    		wxsSizerExtraParams::Left,
    		wxsSizerExtraParams::Right,
    		wxsSizerExtraParams::Top,
    		wxsSizerExtraParams::Bottom
    	};

    	PGId = Grid->Append(wxFlagsProperty(Name,wxPG_LABEL,Borders,Values,0,BorderFlags));
        Grid->SetPropertyAttribute(PGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    }

    bool wxsBorderProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
        if ( Id == PGId )
        {
        	BorderFlags = Grid->GetPropertyValue(Id).GetLong();
        	return ValueChanged(false);
        }
        return true;
    }

    void wxsBorderProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
        Grid->SetPropertyValue(PGId,BorderFlags);
    }

#endif
