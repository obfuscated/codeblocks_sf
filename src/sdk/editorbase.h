#ifndef EDITORBASE_H
#define EDITORBASE_H

#include <wx/frame.h>
#include <wx/mdi.h>

/** @brief Base class that all "editors" should inherit from. */
class EditorBase : public wxMDIChildFrame
{
    public:
        EditorBase(wxMDIParentFrame* parent, const wxString& title);
        virtual ~EditorBase();

		virtual const wxString& GetFilename(){ return GetTitle(); }
		virtual const wxString& GetTitle();
		virtual void SetTitle(const wxString& newTitle);
		
		// return true if this editor can be closed
		virtual bool QueryClose(){ return true; }
		// return true if editor closed succesfully
		// default implementation, calls Destroy() and returns true
		virtual bool Close();

        virtual wxMDIParentFrame* GetParent();
        bool IsBuiltinEditor(); // deliberately non-virtual
    protected:
        bool m_IsBuiltinEditor; // do not mess with it!
    private:
        wxMDIParentFrame* m_pParent;
        wxString m_WinTitle;
};

#endif // EDITORBASE_H
