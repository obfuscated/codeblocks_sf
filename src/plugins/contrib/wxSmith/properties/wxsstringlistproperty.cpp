#include "wxsstringlistproperty.h"

#include <wx/button.h>
#include <wx/tokenzr.h>

class wxsStringListPropertyWindow: public wxButton
{
	public:
		wxsStringListPropertyWindow(wxWindow* Parent,wxsStringListProperty* Property):
			wxButton(Parent,-1,wxT("Edit"),wxDefaultPosition,wxDefaultSize),
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

	class ListEditor: public wxDialog
	{
		public:
			ListEditor(wxWindow* Parent,wxArrayString& _Array,int* _Selection):
				wxDialog(Parent,-1,wxT("List editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
				Array(_Array),
				Selection(_Selection)
			{
				wxFlexGridSizer* Sizer = new wxFlexGridSizer(1,5,5);
				Sizer->AddGrowableCol(0);
				Sizer->AddGrowableRow(1);
				Sizer->Add(new wxStaticText(this,-1,wxT("List items")),0,wxLEFT|wxRIGHT,5);
				Sizer->Add(List = new wxTextCtrl(this,-1,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE),0,wxLEFT|wxRIGHT|wxGROW,5);
				
				if ( Selection )
				{
					Sizer->Add(new wxStaticText(this,-1,wxT("Selection")),0,wxLEFT|wxRIGHT,5);
					Sizer->Add(Selected = new wxChoice(this,-1),0,wxLEFT|wxRIGHT,5);
				}
				
				wxBoxSizer* Internal = new wxBoxSizer(wxHORIZONTAL);
				Internal->Add(new wxButton(this,wxID_OK,wxT("OK")),1,wxLEFT|wxRIGHT,5);
				Internal->Add(new wxButton(this,wxID_CANCEL,wxT("Cancel")),1,wxLEFT|wxRIGHT,5);

				Sizer->Add(Internal,0,wxGROW|wxLEFT|wxRIGHT|wxBOTTOM,5);
				
				SetSizer(Sizer);
				Sizer->SetSizeHints(this);
			
				CenterOnScreen();
				
				for ( int i=0; i<(int)Array.Count(); i++ )
				{
					List->AppendText(Array[i]);
					List->AppendText(wxT("\n"));
				}
			}
			
		private:
		
			void OnListChanged(wxCommandEvent& event)
			{
				if ( Selection != NULL )
				{
					// TODO (SpOoN#1#): Update content of Selection
				}
			}
			
			void OnStore(wxCommandEvent& event)
			{
				wxStringTokenizer Tokenizer(List->GetValue(),wxT("\n"));
				Array.Clear();
				while ( Tokenizer.HasMoreTokens() )
				{
					Array.Add(Tokenizer.GetNextToken());
				}
				event.Skip();
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


wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array):
	wxsProperty(Properties),
	Array(_Array),
	Selected(NULL)
{}

wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array,int& _Selected):
	wxsProperty(Properties),
	Array(_Array),
	Selected(&_Selected)
{}

wxsStringListProperty::~wxsStringListProperty()
{}

const wxString& wxsStringListProperty::GetTypeName()
{
    static wxString Name(wxT("wxArrayString"));
    return Name;
}

wxWindow* wxsStringListProperty::BuildEditWindow(wxWindow* Parent)
{
	return new wxsStringListPropertyWindow(Parent,this);
}

void wxsStringListProperty::UpdateEditWindow()
{}

void wxsStringListProperty::EditList()
{
	ListEditor Editor(NULL,Array,Selected);
	Editor.ShowModal();
	ValueChanged();
}
