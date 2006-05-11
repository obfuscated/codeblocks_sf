#include "wxswindoweditordragassist.h"

#include <manager.h>

wxsWindowEditorDragAssist::wxsWindowEditorDragAssist(wxsWindowEditorContent* _Content):
    PreviousTarget(NULL),
    PreviousParent(NULL),
    PreviousAddAfter(false),
    TargetBitmap(NULL),
    TargetRect(0,0,0,0),
    IsTarget(false),
    ParentBitmap(NULL),
    ParentRect(0,0,0,0),
    IsParent(false),
    Content(_Content)
{
}

wxsWindowEditorDragAssist::~wxsWindowEditorDragAssist()
{
    if ( TargetBitmap ) delete TargetBitmap;
    if ( ParentBitmap ) delete ParentBitmap;
}

void wxsWindowEditorDragAssist::NewDragging()
{
    PreviousTarget = NULL;
    PreviousParent = NULL;
    if ( TargetBitmap )
    {
        delete TargetBitmap;
        TargetBitmap = NULL;
    }
    if ( ParentBitmap )
    {
        delete ParentBitmap;
        ParentBitmap = NULL;
    }
    IsTarget = false;
    IsParent = false;
}

void wxsWindowEditorDragAssist::DrawExtra(wxsItem* Target,wxsItem* Parent,bool AddAfter,wxDC* DC)
{
    UpdateAssist(Target,Parent,AddAfter);

    if ( IsParent )
    {
        switch ( AssistType() )
        {
            case wxsDTSimple:
                DC->SetPen(wxPen(ParentColour(),2,wxSOLID));
                DC->DrawRectangle(ParentRect.x,ParentRect.y,ParentRect.width,ParentRect.height);
                break;

            case wxsDTColourMix:
                DC->DrawBitmap(*ParentBitmap,ParentRect.x,ParentRect.y);
                break;

            default:;
        }
    }

    if ( IsTarget )
    {
        switch ( AssistType() )
        {
            case wxsDTSimple:
                DC->SetPen(wxPen(TargetColour(),2,wxSOLID));
                DC->DrawRectangle(TargetRect.x,TargetRect.y,TargetRect.width,TargetRect.height);
                break;

            case wxsDTColourMix:
                DC->DrawBitmap(*TargetBitmap,TargetRect.x,TargetRect.y);
                break;

            default:;
        }
    }
}

void wxsWindowEditorDragAssist::UpdateAssist(wxsItem* NewTarget,wxsItem* NewParent,bool NewAddAfter)
{
    if ( NewParent != PreviousParent )
    {
        PreviousParent = NewParent;
        RebuildParentAssist();
    }
    if ( NewTarget != PreviousTarget || NewAddAfter != PreviousAddAfter )
    {
        PreviousTarget = NewTarget;
        PreviousAddAfter = NewAddAfter;
        RebuildTargetAssist();
    }

}

void wxsWindowEditorDragAssist::RebuildParentAssist()
{
    int PosX;
    int PosY;
    int SizeX;
    int SizeY;

    if ( PreviousParent && Content->FindAbsoluteRect(PreviousParent,PosX,PosY,SizeX,SizeY) )
    {
        ParentRect = wxRect(PosX,PosY,SizeX,SizeY);

        if ( ParentBitmap )
        {
            delete ParentBitmap;
            ParentBitmap = NULL;
        }

        switch ( AssistType() )
        {
            case wxsDTColourMix:
                {
                    const wxBitmap& Bmp = Content->GetBitmap();
                    UpdateRect(ParentRect,Bmp);
                    wxImage ParentImg = Bmp.GetSubBitmap(ParentRect).ConvertToImage();
                    ColourMix(ParentImg,ParentColour());
                    ParentBitmap = new wxBitmap(ParentImg);
                }
                break;

            default:;
        }

        IsParent = true;
    }
    else
    {
        if ( ParentBitmap )
        {
            delete ParentBitmap;
            ParentBitmap = NULL;
        }
        IsParent = false;
    }
}

void wxsWindowEditorDragAssist::RebuildTargetAssist()
{
    int PosX;
    int PosY;
    int SizeX;
    int SizeY;

    if ( PreviousTarget && Content->FindAbsoluteRect(PreviousTarget,PosX,PosY,SizeX,SizeY) )
    {
        SizeX = SizeX/2;
        if ( PreviousAddAfter ) PosX = PosX + SizeX;

        TargetRect = wxRect(PosX,PosY,SizeX,SizeY);

        if ( TargetBitmap )
        {
            delete TargetBitmap;
            TargetBitmap = NULL;
        }

        switch ( AssistType() )
        {
            case wxsDTColourMix:
                {
                    const wxBitmap& Bmp = Content->GetBitmap();
                    UpdateRect(TargetRect,Bmp);
                    wxImage TargetImg = Bmp.GetSubBitmap(TargetRect).ConvertToImage();
                    ColourMix(TargetImg,TargetColour());
                    TargetBitmap = new wxBitmap(TargetImg);
                }
                break;

            default:;
        }

        IsTarget = true;
    }
    else
    {
        if ( TargetBitmap )
        {
            delete TargetBitmap;
            TargetBitmap = NULL;
        }
        IsTarget = false;
    }
}

inline int wxsWindowEditorDragAssist::AssistType()
{
    return wxsDWAssistType;
}

inline wxColour wxsWindowEditorDragAssist::TargetColour()
{
    int Col = wxsDWTargetCol;
    return wxColour((Col>>16)&0xFF,(Col>>8)&0xFF,Col&0xFF);
}

inline wxColour wxsWindowEditorDragAssist::ParentColour()
{
    int Col = wxsDWParentCol;
    return wxColour((Col>>16)&0xFF,(Col>>8)&0xFF,Col&0xFF);
}

void wxsWindowEditorDragAssist::ColourMix(wxImage& Image,const wxColour& Col)
{
    int R = Col.Red();
    int G = Col.Green();
    int B = Col.Blue();
    for ( int y = Image.GetHeight(); y-->0; )
        for ( int x = Image.GetWidth(); x-->0; )
        {
            Image.SetRGB(x,y,
                ((int)Image.GetRed(x,y)   + R ) / 2,
                ((int)Image.GetGreen(x,y) + G ) / 2,
                ((int)Image.GetBlue(x,y)  + B ) / 2);
        }
}

void wxsWindowEditorDragAssist::UpdateRect(wxRect& Rect,const wxBitmap& Bmp)
{
    if ( Rect.x < 0 )
    {
        Rect.width += Rect.x;
        Rect.x = 0;
    }
    if ( Rect.y < 0 )
    {
        Rect.height += Rect.y;
        Rect.y = 0;
    }

    if ( Rect.width > Bmp.GetWidth() )
    {
        Rect.width = Bmp.GetWidth();
    }

    if ( Rect.height > Bmp.GetHeight() )
    {
        Rect.height = Bmp.GetHeight();
    }
}
