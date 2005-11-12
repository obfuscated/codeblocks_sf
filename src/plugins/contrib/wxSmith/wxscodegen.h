#ifndef WXSCODEGEN_H
#define WXSCODEGEN_H

#include "widget.h"


class WXSCLASS wxsCodeGen
{
    public:

        wxsCodeGen(wxsWidget* Widget,bool DontCreateRoot = true);
        virtual ~wxsCodeGen();

        inline const wxString& GetCode()
        {
            return Code;
        }

    protected:

        /** This function should make the code more beauty ;) */
        virtual void BeautyCode(wxString& Code);

    private:
        wxString Code;

        void AppendCodeReq(wxsWidget* Widget,wxsCodeParams& Params);
};
#endif // WXSCODEGEN_H
