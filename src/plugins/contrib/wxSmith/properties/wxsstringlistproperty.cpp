#include "../wxsheaders.h"
#include "wxsstringlistproperty.h"

#include <wx/button.h>
#include <wx/tokenzr.h>

#ifdef __NO_PROPGRGID

    class WXSCLASS wxsStringListPropertyWindow: public wxButton
    {
        public:
            wxsStringListPropertyWindow(wxWindow* Parent,wxsStringListProperty* Property):
                wxButton(Parent,-1,_("Edit"),wxDefaultPosition,wxDefaultSize),
                Prop(Property)
            {}

        private:

            void OnClick(wxCommandEvent& event)
            {
                if ( Prop ) Prop->EditList();
            }

            wxsStringListProperty* Prop;

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsStringListPropertyWindow,wxButton)
        EVT_BUTTON(-1,wxsStringListPropertyWindow::OnClick)
    END_EVENT_TABLE()


    namespace {

        class WXSCLASS ListEditor: public wxDialog
        {
            public:
                ListEditor(wxWindow* Parent,wxArrayString& _Array,int* _Selection):
                    wxDialog(Parent,-1,_("List editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
                    Array(_Array),
                    Selection(_Selection)
                {
                    wxFlexGridSizer* Sizer = new wxFlexGridSizer(1,5,5);
                    Sizer->AddGrowableCol(0);
                    Sizer->AddGrowableRow(1);
                    Sizer->Add(new wxStaticText(this,-1,_("List items")),0,wxLEFT|wxRIGHT,5);
                    Sizer->Add(List = new wxTextCtrl(this,-1,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE),0,wxLEFT|wxRIGHT|wxGROW,5);

                    if ( Selection )
                    {
                        Sizer->Add(new wxStaticText(this,-1,_("Selection")),0,wxLEFT|wxRIGHT,5);
                        Sizer->Add(Selected = new wxChoice(this,-1),0,wxLEFT|wxRIGHT|wxGROW,5);
                    }

                    wxBoxSizer* Internal = new wxBoxSizer(wxHORIZONTAL);
                    Internal->Add(new wxButton(this,wxID_OK,_("OK")),1,wxLEFT|wxRIGHT,5);
                    Internal->Add(new wxButton(this,wxID_CANCEL,_("Cancel")),1,wxLEFT|wxRIGHT,5);

                    Sizer->Add(Internal,0,wxGROW|wxLEFT|wxRIGHT|wxBOTTOM,5);

                    CenterOnScreen();

                    for ( int i=0; i<(int)Array.Count(); i++ )
                    {
                        List->AppendText(Array[i]);
                        List->AppendText(_T("\n"));
                    }

                    BuildSelection();
                    if ( Selection )
                    {
                        if ( *Selection < 0 || *Selection >= (int)Array.Count() )
                        {
                            Selected->SetSelection(0);
                        }
                        else
                        {
                            Selected->SetSelection(*Selection+1);
                        }
                    }

                    SetSizer(Sizer);
                    Sizer->SetSizeHints(this);
                }

            private:

                void OnListChanged(wxCommandEvent& event)
                {
                    if ( Selection != NULL )
                    {
                        wxString Item = Selected->GetStringSelection();
                        BuildSelection();
                        int Sel = Selected->FindString(Item);
                        if ( Sel == wxNOT_FOUND ) Sel = 0;
                        Selected->Select(Sel);
                    }
                }

                void OnStore(wxCommandEvent& event)
                {
                    wxStringTokenizer Tokenizer(List->GetValue(),_T("\n"));
                    Array.Clear();
                    while ( Tokenizer.HasMoreTokens() )
                    {
                        Array.Add(Tokenizer.GetNextToken());
                    }
                    if ( Selection )
                    {
                        *Selection = Selected->GetSelection() - 1;
                    }
                    event.Skip();
                }

                void BuildSelection()
                {
                    Selected->Clear();
                    Selected->Append(_("--- NONE ---"));
                    wxStringTokenizer Tokenizer(List->GetValue(),_T("\n"));
                    while ( Tokenizer.HasMoreTokens() )
                    {
                        Selected->Append(Tokenizer.GetNextToken());
                    }
                }

                wxArrayString& Array;
                int* Selection;

                wxTextCtrl* List;
                wxChoice* Selected;

                DECLARE_EVENT_TABLE()
        };

        BEGIN_EVENT_TABLE(ListEditor,wxDialog)
            EVT_TEXT(-1,ListEditor::OnListChanged)
            EVT_BUTTON(wxID_OK,ListEditor::OnStore)
        END_EVENT_TABLE()

    };

#endif

wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array):
	wxsProperty(Properties),
	Array(_Array),
	Selected(NULL),
	SortedFlag(0)
    #ifndef __NO_PROPGRGID
        , PGId(0), SelId(0)
    #endif
{}

wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array,int& _Selected,int _SortedFlag):
	wxsProperty(Properties),
	Array(_Array),
	Selected(&_Selected),
	SortedFlag(_SortedFlag)
    #ifndef __NO_PROPGRGID
        , PGId(0), SelId(0)
    #endif
{}

wxsStringListProperty::~wxsStringListProperty()
{}

const wxString& wxsStringListProperty::GetTypeName()
{
    static wxString Name(_T("wxArrayString"));
    return Name;
}

#ifdef __NO_PROPGRGID

    wxWindow* wxsStringListProperty::BuildEditWindow(wxWindow* Parent)
    {
        return new wxsStringListPropertyWindow(Parent,this);
    }

    void wxsStringListProperty::UpdateEditWindow()
    {}

    void wxsStringListProperty::EditList()
    {
        ListEditor Editor(NULL,Array,Selected);
        if ( Editor.ShowModal() == wxID_OK )
        {
            ValueChanged(true);
            if ( Selected )
            {
                GetProperties()->UpdateProperties();
            }
        }
    }

#else

    void wxsStringListProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
    {
    	PGId = Grid->Append(wxArrayStringProperty(Name,wxPG_LABEL,Array));
    	if ( Selected )
    	{
            SelId = Grid->Append(wxEnumProperty(_("Selection"),wxPG_LABEL));
            BuildChoices(Grid);
            Grid->SetPropertyValue(SelId,*Selected);
    	}
    }

    bool wxsStringListProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
    {
    	if ( Id == PGId )
    	{
    		Array = Grid->GetPropertyValue(Id).GetArrayString();
    		if ( Selected )
    		{
    			RebuildChoices(Grid);
    		}
    		return ValueChanged(true);
        }
        if ( Selected && Id == SelId )
        {
        	*Selected = Grid->GetPropertyValue(SelId).GetLong();
        	return ValueChanged(true);
        }
        return true;
    }

    void wxsStringListProperty::UpdatePropGrid(wxPropertyGrid* Grid)
    {
    	Grid->SetPropertyValue(PGId,Array);
    	if ( Selected )
    	{
    		BuildChoices(Grid);
            Grid->SetPropertyValue(SelId,*Selected);
    	}
    }

    void wxsStringListProperty::BuildChoices(wxPropertyGrid* Grid)
    {
    	wxPGConstants& consts = Grid->GetPropertyChoices(SelId);
    	static const wxChar* None[] = { _("-- None --"), NULL };
    	static long NoneValue[] = { -1 };
    	consts.Set(None,NoneValue);
    	wxArrayString Items = Array;
    	wxArrayInt Values;
    	for ( size_t i = 0; i < Items.Count(); i++ )
    	{
    		Values.Add(i);
    	}
    	if ( IsSorted() )
    	{
    		Items.Sort();
    	}
    	consts.Add(Items,Values);
    }

    void wxsStringListProperty::RebuildChoices(wxPropertyGrid* Grid)
    {
    	long SelectedNum = Grid->GetPropertyValue(SelId).GetLong();
    	if ( SelectedNum != -1 )
    	{
    		wxString SelectedStr = Grid->GetPropertyValue(SelId);
    		BuildChoices(Grid);
    		long Index = -1;
    		if ( IsSorted() )
    		{
                wxArrayString Items = Array;
                Items.Sort();
                Index = Items.Index(SelectedStr);
    		}
    		else
    		{
    			Index = Array.Index(SelectedStr);
    		}

    		if ( Index < 0 || Index >= (long)Array.Count() )
    		{
    			Index = -1;
    		}

    		*Selected = Index;
    		Grid->SetPropertyValue(SelId,Index);
    	}
    	else
    	{
    		BuildChoices(Grid);
    		Grid->SetPropertyValue(SelId,-1);
    	}
    }

    bool wxsStringListProperty::IsSorted()
    {
        assert ( GetProperties() );
        assert ( GetProperties()->GetWidget() );
        return ( GetProperties()->GetWidget()->GetBaseProperties().Style & SortedFlag ) != 0;
    }

#endif
