#ifndef WXSWINDOWEDITORDRAGASSIST_H
#define WXSWINDOWEDITORDRAGASSIST_H

#include "wxswindoweditorcontent.h"
#include "../wxsitem.h"
#include <wx/dc.h>
#include <wx/bitmap.h>

/** \brief Class drawing additional data to help dragging */
class wxsWindowEditorDragAssist
{
    public:

        /** \brief Ctor */
        wxsWindowEditorDragAssist(wxsWindowEditorContent* Content);

        /** \brief Dctor */
        ~wxsWindowEditorDragAssist();

        /** \brief Function starting new dragging process */
        void NewDragging();

        /** \brief Function draging additional stuff */
        void DrawExtra(wxsItem* Target,wxsItem* Parent,bool AddAfter,wxDC* DC);

    private:

        wxsItem* PreviousTarget;
        wxsItem* PreviousParent;
        bool PreviousAddAfter;

        wxBitmap* TargetBitmap;
        wxRect    TargetRect;
        bool      IsTarget;

        wxBitmap* ParentBitmap;
        wxRect    ParentRect;
        bool      IsParent;

        wxsWindowEditorContent* Content;

        void UpdateAssist(wxsItem* NewTarget,wxsItem* NewParent,bool NewAddAfter);
        void RebuildParentAssist();
        void RebuildTargetAssist();
        inline int AssistType();
        inline wxColour TargetColour();
        inline wxColour ParentColour();
        void ColourMix(wxImage& Image,const wxColour& Colour);
        void UpdateRect(wxRect& Rect,const wxBitmap& bmp);
};

#endif
