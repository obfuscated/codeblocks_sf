#ifndef WXSSTDDIALOGBUTTONSIZER_H
#define WXSSTDDIALOGBUTTONSIZER_H

#include "../wxsdefsizer.h"

wxsDSDeclareBegin(StdDialogButtonSizer,wxsStdDialogButtonSizerId)
/* TODO (TakeshiMiya##): These bool/checkboxs are not used for now...
    ...because I don't know how to do this:
    1) Create buttons on the fly, from toggling these checkbox properties.
    2) The buttons itself should not permit change their own ID name/value.
       But they should permit changing all other button properties.
    3) This sizer only should supports buttons. A message box should error if the user
       tries to add any other widget than a button.
    4) Buttons must not can be added by the user, the buttons should only be created by
       this sizer. A message box should error here also.
    5) Some options are mutually exclusive, namely:
       (wxID_OK, wxID_YES, wxID_SAVE) and (wxID_HELP, wxID_CONTEXT_HELP), so for example,
       it shouldn't be permitted to have an OK and a Save button at the same time.

    The IDs supported by wxStdDialogButtonSizer are:
    wxID_OK
    wxID_YES
    wxID_SAVE
    wxID_APPLY
    wxID_NO
    wxID_CANCEL
    wxID_HELP
    wxID_CONTEXT_HELP
*/

    virtual bool XmlLoadChildren() { return true; }
    virtual bool XmlSaveChildren() { return true; }
    virtual bool CanAddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1) { return false; }
    virtual int AddChild(wxsWidget* NewWidget,int InsertBeforeThis=-1) { return -1; }

    bool chkOK;
    bool chkYES;
    bool chkSAVE;
    bool chkAPPLY;
    bool chkNO;
    bool chkCANCEL;
    bool chkHELP;
    bool chkCONTEXT_HELP;
    wxString strOK;
    wxString strYES;
    wxString strSAVE;
    wxString strAPPLY;
    wxString strNO;
    wxString strCANCEL;
    wxString strHELP;
    wxString strCONTEXT_HELP;

    wxString GetLabel(TiXmlElement* button);
    void AddButton(wxString Id,wxString Label);
wxsDSDeclareEnd()

#endif // WXSSTDDIALOGBUTTONSIZER_H
