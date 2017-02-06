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

class GotoHandler : public wxEvtHandler
{
    public:
        GotoHandler(wxDialog* parent, GotoFileIterator *iterator);
        ~GotoHandler();
        void Init(wxListCtrl *list, wxTextCtrl *text);
        void DeInit(wxWindow *window);
    private:
        void FilterItems();
    private:
        void OnKeyDown(wxKeyEvent& event);
        void OnTextChanged(wxCommandEvent& event);
    private:
        wxDialog *m_parent;
        wxListCtrl *m_list;
        wxTextCtrl *m_text;
        GotoFileIterator *m_iterator;
};


class GotoFileListCtrl;

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
        //*)

    protected:

        void BuildContent(wxWindow* parent, GotoFileIterator *iterator);

        DECLARE_EVENT_TABLE()
};

#endif
