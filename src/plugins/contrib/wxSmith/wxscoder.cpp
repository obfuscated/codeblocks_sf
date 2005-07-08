#include "wxscoder.h"

#include <manager.h>
#include <editormanager.h>

#include <messagemanager.h>

#define DebLog Manager::Get()->GetMessageManager()->DebugLog

wxsCoder::wxsCoder():
	Enteries(NULL),
	BlockProcess(false)
{
}

wxsCoder::~wxsCoder()
{
	ProcessCodeQueue();
}

void wxsCoder::AddCode(const wxString& FileName,const wxString& BlockHeader,const wxString& Code)
{
	DataMutex.Lock();
	
	// iterating through all enteries searching for currently associated code

	CodeEntry* Entry;
	CodeEntry* Previous;
	for ( Entry = Enteries, Previous = NULL;
	      Entry != NULL;
	      Previous = Entry, Entry = Entry->Next )
	{
		if ( Entry->FileName == FileName && Entry->BlockHeader == BlockHeader )
			break;
	}
	
	if ( Entry )
	{
		( Previous ? Previous->Next : Enteries ) = Entry->Next;		
		Previous = Entry;
		if ( Previous ) while ( Previous->Next ) Previous = Previous->Next;
	}
	else
	{
		Entry = new CodeEntry;
		Entry->FileName = FileName;
		Entry->BlockHeader = BlockHeader;
	}

	Entry->Code = Code;
	time(&Entry->TimeStamp);
	Entry->Next = NULL;
	( Previous ? Previous->Next : Enteries ) = Entry;
	DataMutex.Unlock();
	
	
// TODO (SpOoN#1#): Add sheduling and replace direct processing
	
	ProcessCodeQueue();
}

bool wxsCoder::ProcessCodeQueue()
{
	wxMutexLocker Locker(DataMutex);
	
	if ( BlockProcess ) return false;
	BlockProcess = true;
	
	EditorManager* EM = Manager::Get()->GetEditorManager();
	
	assert ( EM != NULL );
	bool Result = true;
	time_t now;
	time(&now);

// TODO (SpOoN#1#): Process requests which are staying inside queue long enough
	while ( Enteries )
	{
		CodeEntry* Entry = Enteries;
		cbEditor* Editor = EM->GetBuiltinEditor(Entry->FileName);
		if ( Editor )
		{
			if ( !ApplyChanges(Entry,Editor) ) Result = false;
		}
		else
		{
			if ( !ApplyChanges(Entry,Entry->FileName) ) Result = false;
		}

		Enteries = Enteries->Next;
		delete Entry;
	}
	
	BlockProcess = false;
	return Result;
}

void wxsCoder::DropQueue()
{
	wxMutexLocker Locker(DataMutex);

	for ( CodeEntry* Entry = Enteries, *Next; Entry; Entry = Next )
	{
		Next = Entry->Next;
		delete Entry;
	}
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,cbEditor* Editor)
{
	assert ( Editor != NULL );
	cbStyledTextCtrl* Ctrl = Editor->GetControl();
	assert ( Ctrl != NULL );
	
	Ctrl->SetSearchFlags(wxSTC_FIND_MATCHCASE);
	
	Ctrl->SetTargetStart(0);
	Ctrl->SetTargetEnd(Ctrl->GetLength());	// TODO: Find if this works
	
	int Position = Ctrl->SearchInTarget(Entry->BlockHeader);
	
	if ( Position == -1 )
	{
		wxMessageBox(wxString::Format(
			wxT("Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Entry->BlockHeader.c_str(),
			Editor->GetFilename().c_str()));
		return false;
	}
	else
	{
		// Beginning of this code block is in Position, now searching for end
		Ctrl->SetTargetStart(Position);
		Ctrl->SetTargetEnd(Ctrl->GetLength());
		int End = Ctrl->SearchInTarget(wxT("//*)"));
		if ( End == -1 )
		{
			wxMessageBox(wxString::Format(
				wxT("Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
				Entry->BlockHeader.c_str(),
				Editor->GetFilename().c_str()));
			return false;
		}
		else
		{
			Ctrl->SetTargetStart(Position);
			Ctrl->SetTargetEnd(End);
			Ctrl->ReplaceTarget(Entry->Code);
			Editor->SetModified();
		}
	}
		
	return true;
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,const wxString& FileName)
{
// TODO (SpOoN#1#): Operate on files, not cbEditor
	DebLog("Applying code changes to: %s",FileName.c_str());
	cbEditor* Editor = Manager::Get()->GetEditorManager()->Open(FileName);
	
	if ( !Editor )
	{
		wxMessageBox(wxString::Format(wxT("Couldn't open file : '%s'"),FileName.c_str()));
		return false;
	}
	
	ApplyChanges(Entry,Editor);
	Editor->Save();
	Editor->Close();
	return true;
}

bool wxsCoder::ProcessCodeForFile(const wxString& FileName)
{
	wxMutexLocker Locker(DataMutex);

	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
	
	bool Result = true;
	
	for ( CodeEntry* Entry = Enteries, *Previous = NULL; Entry; )
	{
		if ( Entry->FileName == FileName )
		{
			cbEditor* Editor = EM->GetBuiltinEditor(Entry->FileName);
			if ( Editor )
			{
				if ( !ApplyChanges(Entry,Editor) ) Result = false;
			}
			else
			{
				if ( !ApplyChanges(Entry,Entry->FileName) ) Result = false;
			}
			
			CodeEntry* Next = ( Previous ? Previous->Next : Enteries ) = Entry->Next;
			delete Entry;
			Entry = Next;
		}
		else
		{
			Previous = Entry;
			Entry = Entry->Next;
		}
	}
	
	return Result;
}

static wxsCoder SingletonObject;
wxsCoder* wxsCoder::Singleton = &SingletonObject;

