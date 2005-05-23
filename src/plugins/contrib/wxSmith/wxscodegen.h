#ifndef WXSCODEGEN_H
#define WXSCODEGEN_H

#include "widget.h"


class wxsCodeGen
{
    public:
    
        wxsCodeGen(wxsWidget* Widget,int InitialSpaces=0,int TabSize=4);
        virtual ~wxsCodeGen();
    
        inline const wxString& GetCode()
        {
            return Code;
        }
    
    protected:
    
        /** This function should make the code more beauty ;) */
        virtual void BeautyCode(wxString& Code,int InitialSpaces,int TabSize);
    
    private:
        wxString Code;
    
        void AppendCodeReq(wxsWidget* Widget,wxsCodeParams& Params);
};
#endif // WXSCODEGEN_H
