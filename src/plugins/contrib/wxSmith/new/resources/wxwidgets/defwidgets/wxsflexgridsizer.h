#ifndef WXSFLEXGRIDSIZER_H
#define WXSFLEXGRIDSIZER_H

#include "../wxssizer.h"

class wxsFlexGridSizer: public wxsSizer
{
    public:
        static wxsItemInfo Info;

        wxsFlexGridSizer(wxsWindowRes* Resource):
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
        wxString GrowableRows;
        wxString GrowableCols;

        static wxArrayInt GetArray(const wxString& String,bool* Valid = NULL);
        static bool FixupList(wxString& List);
};

#endif
