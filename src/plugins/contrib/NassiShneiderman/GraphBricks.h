#ifndef _GRAPH_BRICK_H_INCLUDED
#define _GRAPH_BRICK_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "TextGraph.h"

#include <map>


class NassiView;
class NassiBrick;
class GraphNassiBrick;
class TextGraph;
class HooverDrawlet;


typedef std::map<NassiBrick*, GraphNassiBrick*> BricksMap;
//typedef std::map<const wxString*, TextGraph*> MultilineGraphMap;
//typedef std::vector<wxPoint> PointVector;

class GraphNassiBrick
{
public:
    GraphNassiBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
    virtual ~GraphNassiBrick(void);

private:
    GraphNassiBrick(const GraphNassiBrick& p);
    GraphNassiBrick &operator=(const GraphNassiBrick &rhs);

public:
    class Position
    {
        public:
        enum pos_enum{
            top,
            bottom,
            child,
            childindicator,
            none
        };
        pos_enum pos;
        wxUint32 number;
    };

public:
    void SetInvisible(bool invis);
    bool IsVisible();

    virtual void Draw(wxDC *dc);// = 0;
    virtual void CalcMinSize(wxDC* /*dc*/, wxPoint& /*size*/){}// = 0;
    virtual void SetOffsetAndSize(wxDC* /*dc*/, wxPoint /*offset*/, wxPoint /*size*/){}
    virtual wxUint32 GetWidth();
    virtual wxUint32 GetHeight();
    virtual wxPoint GetOffset();
    virtual wxUint32 GetMinimumWidth();
    virtual wxUint32 GetMinimumHeight();
    virtual bool IsMinimized(){return false;}

    virtual bool HasPoint(const wxPoint &pos);
    virtual TextGraph *IsOverText(const wxPoint &pos) = 0;
    virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
    NassiBrick *GetBrick(){ return m_brick; }

    virtual bool IsOverChildIndicator(const wxPoint& /*pos*/, wxUint32* /*childNumber*/ = 0){return false;}
    virtual wxUint32 ActiveChildIndicator(){return 0;}
    virtual bool SeparatesChildsVertically(){return true;}
    virtual bool HasActiveChildIndicator(){return false;}

    virtual HooverDrawlet *GetDrawlet(const wxPoint &pos, bool HasNoBricks = false);
    virtual GraphNassiBrick::Position GetPosition(const wxPoint &pos);

    virtual bool CanChangeNumberOfChilds(){return false;}

protected:
    NassiBrick *m_brick;
    NassiView *m_view;
    wxPoint m_offset;
    wxPoint m_size;
    wxPoint m_minimumsize;
    bool m_visible;

public:
    void SetActive(bool act = true, bool withChilds = false);
    bool IsActive(){return m_active;}
    virtual void SetChildIndicatorActive(bool /*act*/ = true, wxUint32 /*child*/ = 0){}
private:
    bool m_active;
public:
    virtual void DrawActive(wxDC *dc);

public:
    void SetUsed(bool used = true){m_used = used;}
    bool IsUsed(){return m_used;}
protected:
    GraphNassiBrick *GetGraphBrick(NassiBrick *brick);
private:
    bool m_used;
protected:
    BricksMap *m_map;
};

class GraphNassiMinimizableBrick : public GraphNassiBrick
{
public:
    GraphNassiMinimizableBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
    virtual ~GraphNassiMinimizableBrick(void){}

private:
    GraphNassiMinimizableBrick(const GraphNassiMinimizableBrick& p);

public:
    virtual bool IsMinimized() { return m_minimized; }
    virtual void Minimize(bool min = true){ m_minimized = min;}
    virtual bool HasPoint(const wxPoint &pos) = 0;
    virtual TextGraph *IsOverText(const wxPoint &pos) = 0;
    virtual bool IsOverMinMaxBox(const wxPoint &pos);
    virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0) = 0;
    virtual HooverDrawlet *GetDrawlet(const wxPoint &pos, bool HasNoBricks = false);
    virtual GraphNassiBrick::Position GetPosition(const wxPoint &pos);
private:
    bool m_minimized;

public:
    void DrawMinMaxBox(wxDC *dc);
private:
    void DrawMinBox(wxDC *dc);
    void DrawMaxBox(wxDC *dc);
};
class GraphNassiInstructionBrick : public GraphNassiBrick
{
    public:
        GraphNassiInstructionBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiInstructionBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual TextGraph *IsOverText(const wxPoint &pos);
    private:
        TextGraph comment;
        TextGraph source;
};
class GraphNassiIfBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiIfBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiIfBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
        //virtual HooverDrawlet *GetDrawlet(const wxPoint &pos, bool HasNoBricks = false);
        virtual GraphNassiBrick::Position GetPosition(const wxPoint &pos);
    private:
        TextGraph commentHead, commentTrue, commentFalse;
        TextGraph source;
    protected:
        wxCoord m_p, m_hh;
};
class GraphNassiWhileBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiWhileBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiWhileBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
    private:
        TextGraph comment;
        TextGraph source;
    private:
        wxCoord m_hh, m_bb;
};
class GraphNassiDoWhileBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiDoWhileBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiDoWhileBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
    private:
        TextGraph comment;
        TextGraph source;
    private:
        wxCoord m_bb, m_hh;
};
class GraphNassiSwitchBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiSwitchBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiSwitchBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
        virtual bool HasActiveChildIndicator();
        virtual bool IsOverChildIndicator(const wxPoint & pos, wxUint32 *childNumber = 0);

        virtual void SetChildIndicatorActive(bool act = true, wxUint32 child = 0);
        virtual wxUint32 ActiveChildIndicator();
        virtual void DrawActive(wxDC *dc);
        virtual HooverDrawlet *GetDrawlet(const wxPoint &pos, bool HasNoBricks = false);
        virtual GraphNassiBrick::Position GetPosition(const wxPoint &pos);
        virtual bool CanChangeNumberOfChilds(){return true;}
    private:
        TextGraph comment;
        TextGraph source;
        std::vector<const wxString *> childcomment;
        std::vector<const wxString *> childsource;
        typedef std::map<const wxString *, TextGraph*> TextGraphMap;
        TextGraphMap m_textMap;
        TextGraph *childcomments(wxUint32 n);
        TextGraph *childsources(wxUint32 n);

        std::vector<wxCoord> minimumHeightOfChildBricks;
        std::vector<wxCoord> offsetToChild;
        std::vector<wxCoord> offsetToChildSeparator;
        std::vector<wxCoord> heightOfChildBricks;
        wxCoord m_b, m_hw;
        bool m_ChildIndicatorIsActive;
        wxUint32 m_ActiveChildIndicator;
};
class GraphNassiBlockBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiBlockBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiBlockBrick();

        wxString GetSource();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
    private:
        wxCoord m_hh;
};
class GraphNassiForBrick : public GraphNassiMinimizableBrick
{
    public:
        GraphNassiForBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiForBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual bool HasPoint(const wxPoint &pos);
        virtual TextGraph *IsOverText(const wxPoint &pos);
        virtual bool IsOverChild(const wxPoint &pos, wxRect *childRect = 0, wxUint32 *childNumber = 0);
    private:
        TextGraph comment;
        TextGraph source;
    private:
        wxCoord m_hh, m_bb, m_b;
};

class GraphNassiBreakBrick : public GraphNassiBrick
{
    public:
        GraphNassiBreakBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiBreakBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual TextGraph *IsOverText(const wxPoint &pos);
    private:
        TextGraph comment;
        wxCoord m_b;
};
class GraphNassiContinueBrick : public GraphNassiBrick
{
    public:
        GraphNassiContinueBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiContinueBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual TextGraph *IsOverText(const wxPoint &pos);
    private:
        TextGraph comment;
        wxCoord m_h;
};
class GraphNassiReturnBrick : public GraphNassiBrick
{
    public:
        GraphNassiReturnBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap);
        virtual ~GraphNassiReturnBrick();

    public:
        virtual void Draw(wxDC *dc);
        virtual void CalcMinSize(wxDC *dc, wxPoint &size);
        virtual void SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size);
        virtual TextGraph *IsOverText(const wxPoint &pos);
    private:
        TextGraph comment;
        TextGraph source;
        wxCoord m_h;
};


#endif
