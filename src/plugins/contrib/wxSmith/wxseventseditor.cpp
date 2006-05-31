#include "wxsheaders.h"
#include "wxseventseditor.h"

#include "widget.h"
#include "wxswidgetevents.h"
#include "resources/wxswindowres.h"
#include "wxsglobals.h"
#include <editormanager.h>

#define NoneStr   _("-- None --")
#define AddNewStr _("-- Add new handler --")


wxsEventsEditor::wxsEventsEditor(wxWindow* Parent,wxsWidget* Widget):
    wxPropertyGrid(Parent,-1,wxDefaultPosition,wxDefaultSize)
{
	SetWidget(Widget);
}

void wxsEventsEditor::SetWidget(wxsWidget* _Widget)
{
	Widget = _Widget;
	Events = _Widget->GetEvents();
	Res = _Widget->GetResource();
	Proj = Res->GetProject();
	if ( Proj )
	{
        SourceFile = Proj->GetProjectFileName(Res->GetSourceFile());
        HeaderFile = Proj->GetProjectFileName(Res->GetHeaderFile());
        ClassName = Res->GetClassName();
        BuildPropertyGrid();
	}
}

wxsEventsEditor::~wxsEventsEditor()
{
}

void wxsEventsEditor::OnPropertyChanged(wxPropertyGridEvent& event)
{
	ReadPropertyGrid();
	Res->UpdateEventTable(true);
	Refresh();
}

void wxsEventsEditor::BuildPropertyGrid(bool UpdateOnly)
{
	UpdateOnly = false;     // There's something wrong when updating
	if ( !UpdateOnly )
	{
		Clear();
	}
	int Cnt = Events->GetEventCount();
	for ( int i=0; i<Cnt; i++ )
	{
		wxsEventDesc* Event = Events->GetEvent(i);

		wxArrayString Functions;
		FindFunctions(Event->EventTypeName,Functions);

        wxPGConstants Const;
        int Index = 0;
        Const.Add(NoneStr,0);
        Const.Add(AddNewStr,1);

        for ( int j=0; j<(int)Functions.Count(); j++ )
        {
        	Const.Add(Functions[j],j+2);
        	if ( Functions[j] == Event->FunctionName )
        	{
        		Index = j+2;
        	}
        }

        if ( UpdateOnly )
        {
        	wxPGId Id = GetPropertyByLabel(Event->EventEntry);
        	if ( Id.IsOk() )
        	{
                SetPropertyChoices(Id,Const);
                SetPropertyValue(Id,Index);
        	}
        }
        else
        {
            Append(wxEnumProperty(Event->EventEntry,wxPG_LABEL,Const,Index));
        }
	}
}

void wxsEventsEditor::ReadPropertyGrid()
{
	bool NeedUpdate = false;
	int Cnt = Events->GetEventCount();
	for ( int i = 0; i < Cnt; ++i )
	{
		wxsEventDesc* Event = Events->GetEvent(i);
		wxPGId Id = GetPropertyByLabel(Event->EventEntry);
		if ( Id.IsOk() )
		{
			wxString Selection = GetPropertyValueAsString(Id);

			if ( Selection == NoneStr )
			{
				Event->FunctionName = _T("");
			}
			else if ( Selection == AddNewStr )
			{
				Event->FunctionName = GetNewFunction(Event);
				NeedUpdate = true;
			}
			else
			{
				Event->FunctionName = Selection;
			}
		}
	}

	if ( NeedUpdate )
	{
		UpdatePropertyGrid();
	}
}

void wxsEventsEditor::FindFunctions(const wxString& EventType,wxArrayString& Array)
{
	wxString Code = wxsCoder::Get()->GetCode( HeaderFile,
        wxString::Format(wxsBHeaderF("Handlers"),ClassName.c_str()) );

    // Basic parsing

    for(;;)
    {
    	// Searching for void statement - it may begin new fuunction declaration

    	int Pos = Code.Find(_T("void"));
    	if ( Pos == -1 ) break;

    	// Removing all before function name
    	Code.Remove(0,Pos+4).Trim(false);

    	// Getting function name
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) )
            {
            	break;
            }
            Pos++;
    	}
    	wxString NewFunctionName = Code.Mid(0,Pos);
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	// Parsing arguments

    	if ( Code.GetChar(0) != _T('(') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	// Getting argument type
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) )
            {
            	break;
            }
            Pos++;
    	}
    	wxString NewEventType = Code.Mid(0,Pos);
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	// Checking if the rest of declaratin is valid
    	if ( Code.GetChar(0) != _T('&') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	// Skipping argument name
    	Pos  = 0;
    	while ( (int)Code.Length() > Pos )
    	{
    		wxChar First = Code.GetChar(Pos);
    		if ( ( First<_T('A') || First>_T('Z') ) &&
    		     ( First<_T('a') || First>_T('z') ) &&
    		     ( First<_T('0') || First>_T('9') ) )
            {
            	break;
            }
            Pos++;
    	}
    	Code.Remove(0,Pos).Trim(false);
    	if ( !Code.Length() ) break;

    	if ( Code.GetChar(0) != _T(')') ) continue;
    	Code.Remove(0,1).Trim(false);
    	if ( !Code.Length() ) break;

    	if ( Code.GetChar(0) != _T(';') ) continue;
    	Code.Remove(0,1).Trim(false);

    	if ( NewFunctionName.Length() == 0 || NewEventType.Length() == 0 ) continue;

    	// We got new function, checking event type and adding to array

    	if ( !EventType.Length() || EventType == NewEventType )
    	{
    		Array.Add(NewFunctionName);
    	}
    }
}

wxString wxsEventsEditor::GetFunctionProposition(wxsEventDesc* Event)
{
    // Creating proposition of new function name

    wxString NewNameBase;
    NewNameBase.Printf(_T("On%s%s"),Widget->BaseProperties.VarName.c_str(),Event->NewFuncNameBase.c_str());

    int Suffix = 0;
    wxArrayString Functions;
    FindFunctions(_T(""),Functions);
    wxString NewName = NewNameBase;

    while ( Functions.Index(NewName) != wxNOT_FOUND )
    {
    	NewName = NewNameBase;
    	NewName.Append(wxString::Format(_T("%d"),++Suffix));
    }

    return NewName;
}

wxString wxsEventsEditor::GetNewFunction(wxsEventDesc* Event)
{
	wxString Name = GetFunctionProposition(Event);

	for (;;)
	{
		Name = ::wxGetTextFromUser(_("Enter name for new handler:"),_("New handler"),Name);
		if ( !Name.Length() ) return _T("");

        if ( !wxsValidateIdentifier(Name) )
        {
        	wxMessageBox(_("Invalid name"));
        	continue;
        }

        wxArrayString Functions;
        FindFunctions(_T(""),Functions);

        if ( Functions.Index(Name) != wxNOT_FOUND )
        {
        	wxMessageBox(_("Handler with this name already exists"));
        	continue;
        }

        break;
	}

	// Creating new function

	if ( !CreateNewFunction(Event,Name) )
	{
		wxMessageBox(_("Couldn't add new handler"));
		return _T("");
	}

	return Name;
}

bool wxsEventsEditor::CreateNewFunction(wxsEventDesc* Event,const wxString& NewFunctionName)
{
	wxString DeclarationsHeader;
	DeclarationsHeader.Printf(wxsBHeaderF("Handlers"),ClassName.c_str());
	wxString Declarations = wxsCoder::Get()->GetCode(HeaderFile,DeclarationsHeader);
	if ( Declarations.Length() == 0 ) return false;
	Declarations << _T("void ") << NewFunctionName << _T('(');
	Declarations << Event->EventTypeName << _T("& event);\n");
	wxsCoder::Get()->AddCode(HeaderFile,DeclarationsHeader,Declarations,true);
	Res->UpdateEventTable(true);
	cbEditor* Editor = Manager::Get()->GetEditorManager()->Open(SourceFile);
	if ( !Editor ) return false;
	wxString NewFunctionCode;
	NewFunctionCode.Printf(
        _T("\n")
        _T("void %s::%s(%s& event)\n")
        _T("{\n")
        _T("}\n"),
            Res->GetClassName().c_str(),
            NewFunctionName.c_str(),
            Event->EventTypeName.c_str());
    cbStyledTextCtrl* Ctrl = Editor->GetControl();
    Ctrl->DocumentEnd();
    Ctrl->AddText(NewFunctionCode);
    Ctrl->LineUp();
    Ctrl->LineUp();
    Ctrl->LineEnd();

    Editor->SetModified();
    return true;
}

BEGIN_EVENT_TABLE(wxsEventsEditor,wxPropertyGrid)
    EVT_PG_CHANGED(-1,wxsEventsEditor::OnPropertyChanged)
END_EVENT_TABLE()
