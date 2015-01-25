#ifndef __NASSI_EDITOR_PANEL_H__
#define __NASSI_EDITOR_PANEL_H__


#include "cbEditorPanel.h"
#include "NassiView.h"

class NassiDiagramWindow;

class NassiEditorPanel: public cbEditorPanel , public FileContentObserver
{
    public:
        NassiEditorPanel( const wxString &fileName, const wxString &title );
        virtual ~NassiEditorPanel();
    private:
        NassiEditorPanel(const NassiEditorPanel &p);
        NassiEditorPanel &operator=(const NassiEditorPanel &rhs);
    public:
        virtual void Cut();
        virtual void Copy();
        virtual void Paste();
        virtual void DeleteSelection();
        //virtual bool CanCut() const;
        //virtual bool CanCopy() const;
        virtual bool CanPaste() const;
        virtual bool IsReadOnly() const;
        virtual bool CanSelectAll() const;
        virtual void SelectAll();
        virtual bool HasSelection() const;

    private:
        typedef std::set< EditorBase* > EditorsSet;
        static EditorsSet   m_AllEditors;
    public:
        static void CloseAllNassiEditors();
        static bool IsNassiEditor( EditorBase* editor );

        bool CanZoomIn();
        bool CanZoomOut();
        void ZoomIn();
        void ZoomOut();

        void ExportCSource();
        bool GetCSource(wxTextOutputStream &text_stream, wxUint32 n);
        void ExportVHDLSource();
        void ExportSVG();
        #if wxUSE_POSTSCRIPT
        void ExportPS();
        #endif
        void ExportStrukTeX();
        void ExportBitmap();
        bool CanExport();

        void UpdateColors();
    private:
        NassiView *m_view;
        NassiDiagramWindow *m_diagramwindow;
    public:
        bool IsDrawingSource();
        bool IsDrawingComment();
        void EnableDrawSource(bool en);
        void EnableDrawComment(bool en);
        void ChangeToolTo(NassiView::NassiTools tool);
        void ToolSelect();

    public:
        void Update(wxObject* hint);


    public:
        bool ParseC(const wxString &str);
    //private:
    //    SetFirstbrick(NassiBrick *);

    private:
        DECLARE_EVENT_TABLE()
};

#endif

