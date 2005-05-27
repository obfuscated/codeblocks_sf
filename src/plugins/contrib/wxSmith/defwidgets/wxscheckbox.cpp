#include "wxscheckbox.h"

wxsDWDefineBegin(wxsCheckBox,wxCheckBox,

    ThisWidget = new wxCheckBox(parent,id,label,pos,size,style);
    ThisWidget->SetValue(checked);
    
    )

    wxsDWDefStrP(label,"Label:","");
    wxsDWDefBoolP(checked,"Checked:",false);

wxsDWDefineEnd()
