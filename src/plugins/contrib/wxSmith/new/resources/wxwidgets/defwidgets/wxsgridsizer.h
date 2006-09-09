#ifndef WXSGRIDSIZER_H
#define WXSGRIDSIZER_H

#include "../wxssizer.h"

class wxsGridSizer: public wxsSizer
{
    public:
        static wxsItemInfo Info;

        wxsGridSizer(wxsWindowRes* Resource):
            wxsSizer(Resource),
            Cols(0), Rows(0),
            VGap(0), HGap(0),
            VGapDU(false), HGapDU(false)
        {}

        virtual const wxsItemInfo& GetInfo() { return Info; }

    protected:

        virtual wxSizer* BuildSizerPreview(wxWindow* Parent);
        virtual void BuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void EnumItemProperties(long Flags);
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

    private:

        long Cols;
        long Rows;
        long VGap;
        long HGap;
        bool VGapDU;
        bool HGapDU;
};

#endif
