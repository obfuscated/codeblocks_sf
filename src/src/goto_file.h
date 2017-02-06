#ifndef GOTO_FILE_H
#define GOTO_FILE_H

#ifndef WX_PRECOMP
    //(*HeadersPCH(GotoFile)
    #include <wx/dialog.h>
    class wxBoxSizer;
    class wxListCtrl;
    class wxStaticText;
    class wxTextCtrl;
    //*)
#endif
//(*Headers(GotoFile)
//*)

class GotoFileListCtrl;

class GotoFileIterator
{
    public:
        virtual ~GotoFileIterator() {}

        virtual int GetTotalCount() const = 0;
        virtual int GetFilteredCount() const = 0;
        virtual void Reset() = 0;
        virtual const wxString& GetItemFilterString(int index) const = 0;
        virtual wxString GetDisplayText(int index, int column) const = 0;
        virtual void AddIndex(int index) = 0;

};

class GotoFile: public wxDialog
{
    public:
        GotoFile(wxWindow* parent, GotoFileIterator *iterator);
        ~GotoFile() override;

    private:
        void FilterItems();
    private:
        GotoHandler m_handler;
    private:
        //(*Declarations(GotoFile)
        GotoFileListCtrl* m_ResultList;
        wxTextCtrl* m_Text;
        //*)

        //(*Identifiers(GotoFile)
        static const long ID_TEXTCTRL1;
        static const long ID_RESULT_LIST;
        //*)

        //(*Handlers(GotoFile)
        void OnTextChanged(wxCommandEvent& event);
        //*)

    protected:
        void BuildContent(wxWindow* parent);

        DECLARE_EVENT_TABLE()
};

#endif
