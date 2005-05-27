#ifndef EDITORBASE_H
#define EDITORBASE_H

#include <wx/frame.h>
#include <wx/panel.h>

/** @brief Base class that all "editors" should inherit from. */
class EditorBase : public wxPanel
{
    public:
        EditorBase(wxWindow* parent, const wxString& title);
        virtual ~EditorBase();

		virtual const wxString& GetFilename(){ return GetTitle(); }
		virtual const wxString& GetTitle();
		virtual void SetTitle(const wxString& newTitle);
		
		virtual void Activate();
		
		// return true if this editor can be closed
		virtual bool QueryClose(){ return true; }
		// return true if editor closed succesfully
		// default implementation, calls Destroy() and returns true
		virtual bool Close();

        bool IsBuiltinEditor(); // deliberately non-virtual
    protected:
        bool m_IsBuiltinEditor; // do not mess with it!
    private:
        wxString m_WinTitle;
};

#endif // EDITORBASE_H
