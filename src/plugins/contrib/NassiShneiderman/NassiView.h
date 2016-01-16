#ifndef HEADER_45C21D52F180548F
#define HEADER_45C21D52F180548F


#ifndef NASSIVIEW_H_INCLUDED
#define NASSIVIEW_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dnd.h>

#include "FileContent.h"
#include "GraphBricks.h"

class NassiFileContent;
class NassiDiagramWindow;
class NassiBrick;
class GraphNassiBrick;
class GraphFabric;
class wxCommand;
class HooverDrawlet;
class TextCtrl;

class Task;

struct NassiViewColors
{
    void Init();

    wxColor defaultBrush;
    wxColor emptyBrush;
    wxColor defaultPen;
    wxColor selectionPen;
    wxColor sourceColor;
    wxColor commentColor;
};

class NassiView : public FileContentObserver
{
    public:
        NassiView(NassiFileContent *nfc);
        //NassiView(, wxCommandProcessor *cmdproc);
        ~NassiView();

    public:
        void DrawDiagram(wxDC *dc);

        void DeleteSelection(void);//void DeleteDroppedBricks(void);
        void SelectAll(void);
        void Copy();
        void Cut();
        void Paste();
        void DragStart();
        bool CanPaste()const;
        //bool CanCopy()const;
        //bool CanCut()const;
        bool HasSelection()const;
        bool HasSelectedBricks()const;
        //bool CanDelete();
        //bool IsExportPossible();
        bool CanSelectAll();
        void MoveTextCtrl(const wxPoint &pt );

        /// Called when the environment settings have changed.
        void UpdateColors();
        const NassiViewColors& GetColors() const { return m_colors; }
    private:
        NassiView(const NassiView &p);
        NassiView &operator=(const NassiView &rhs);
        wxCommand *Delete();
        void CopyBricks();
    public:
    enum NassiTools {
        NASSI_TOOL_SELECT      =   0,
        NASSI_TOOL_INSTRUCTION =   1,
        NASSI_TOOL_CONTINUE    =   2,
        NASSI_TOOL_BREAK       =   3,
        NASSI_TOOL_RETURN      =   4,
        NASSI_TOOL_WHILE       =   5,
        NASSI_TOOL_DOWHILE     =   6,
        NASSI_TOOL_FOR         =   7,
        NASSI_TOOL_BLOCK       =   8,
        NASSI_TOOL_IF          =   9,
        NASSI_TOOL_SWITCH      =  10,
        NASSI_TOOL_PASTE       = 100,
        NASSI_TOOL_ESC         = 101
    };

    public:
        void ChangeToolTo(NassiTools tool);
        NassiBrick *GenerateNewBrick(NassiTools tool);
        void NextTool(void);
        void PrevTool(void);
        void ToolSelect();
    private:
        //NassiTools m_tool;
        NassiFileContent *m_nfc;

    public:
        void Update( wxObject* hint = 0 );
    private:
        void UpdateSize();
        static const wxPoint offset;

    public:
        void ZoomIn();
        void ZoomOut();
        bool CanZoomIn();
        bool CanZoomOut();
    private:
        wxInt16 m_fontsize;
        static const wxInt16 FontSizes[38];

    public:
        bool IsDrawingSource();
        bool IsDrawingComment();
        void EnableDrawSource(bool en = true);
        void EnableDrawComment(bool en = true);
        const wxFont &GetCommentFont();
        const wxFont &GetSourceFont();
    private:
        wxFont m_sourcefont;
        wxFont m_commentfont;
        bool m_DrawSource;
        bool m_DrawComment;

    public:
        GraphNassiBrick *GetGraphBrick(NassiBrick *brick);
        GraphNassiBrick *CreateGraphBrick(NassiBrick *brick);
        GraphNassiBrick *GetBrickAtPosition(const wxPoint &position);
//        MultilineTextGraph *GetTextGraph(NassiBrick *brick, wxUint32 nmbr);
//        MultilineTextGraph *CreateTextGraph(NassiBrick *brick, wxUint32 nmbr);
//        MultilineTextGraph *GetTextGraphAtPosition(const wxPoint & pos);
    private:
        BricksMap         m_GraphBricks;
        //MultilineGraphMap m_TextBricks;

    public:
        NassiDiagramWindow *CreateDiagramWindow(wxWindow *parent);
    private:
        NassiDiagramWindow *m_diagramwindow;

    private:
        GraphFabric *m_graphFabric;

    public:
        bool HasUpdated(){return m_updated;}
        void Updated(bool upd = true){ m_updated = upd;}
    private:
        bool m_updated;

    public:
        HooverDrawlet *OnMouseMove(wxMouseEvent &event, const wxPoint &pos);
        void OnMouseLeftDown(wxMouseEvent &event, const wxPoint &pos);
        void OnMouseLeftUp(wxMouseEvent &event, const wxPoint &pos);
        void OnMouseRightDown(wxMouseEvent &event, const wxPoint &pos);
        void OnMouseRightUp(wxMouseEvent& event, const wxPoint &pos);
        void OnKeyDown(wxKeyEvent &event);
        void OnChar(wxKeyEvent &event);

    public:
        void SetTask(Task* task);
    private:
        Task *itsTask;
        void RemoveTask();

    public:
        void SelectFirst(GraphNassiBrick *gfirst);
        void Select(GraphNassiBrick *gbrick, GraphNassiBrick *glast);
        void SelectLast(GraphNassiBrick *gbrick);
        void ClearSelection();
        const wxRect &GetEmptyRootRect();
        void SelectChildIndicator(GraphNassiBrick *gbrick, wxUint32 child);
    private:
        bool hasSelectedBricks;
        bool reverseSelected;
        GraphNassiBrick *firstSelectedGBrick;
        GraphNassiBrick *lastSelectedGBrick;
        GraphNassiBrick *ChildIndicatorParent;
        bool ChildIndicatorIsSelected;
        wxUint32 ChildIndicator;

    private:
        bool cursorOverText;
        wxRect m_EmptyRootRect;

        wxPoint DragStartPoint;
        bool DragPossible;
        bool ThisIsDnDSource;
    public:
        wxDragResult OnDrop(const wxPoint &pt, NassiBrick *brick, wxString strc, wxString strs, wxDragResult def);
        HooverDrawlet *OnDragOver(const wxPoint &pt, wxDragResult &def, bool HasNoBricks);
        void OnDragLeave(void);
        void OnDragEnter(void);

    public:
        #if wxCHECK_VERSION(3, 0, 0)
            void ExportSVG();
        #endif
        void ExportCSource();
        bool ExportCSource(wxTextOutputStream &text_stream, wxUint32 n = 4);
        void ExportVHDLSource();
        #if wxUSE_POSTSCRIPT
            void ExportPS();
        #endif
        void ExportStrukTeX();
        void ExportBitmap();
    public:
        void ShowCaret(bool show = true);
        bool IsCaretVisible();
        void MoveCaret(const wxPoint& pt);
    private:
        TextCtrl *m_txt;


    private:
        NassiViewColors m_colors;
};

#endif




#endif // header guard
