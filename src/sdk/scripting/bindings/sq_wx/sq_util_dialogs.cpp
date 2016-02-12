/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

 #include <wx/numdlg.h>
 #include <wx/colordlg.h>
 #include <wx/textdlg.h>
 #include <wx/filedlg.h>
 #include <wx/dirdlg.h>
 #include <wx/utils.h>
 #include <wx/msgdlg.h>
 #include <scripting/bindings/sq_wx/sq_wx.h>
 #include <scripting/bindings/sc_cb_vm.h>
 #include <scripting/bindings/sc_binding_util.h>



/** \defgroup sq_dialogs Squirrel User dialogs
 *  \ingroup Squirrel
 *  \brief Useful dialogs for interacting with user
 */

namespace ScriptBindings
{
    SQInteger wx_GetColourFromUser(HSQUIRRELVM vm)
    {
        StackHandler sa(vm);
        const wxColour& c = sa.GetParamCount() == 2 ? *sa.GetInstance<wxColour>(2) : *wxBLACK;
        sa.PushInstanceCopy<wxColour>(wxGetColourFromUser(Manager::Get()->GetAppWindow(),c));
        return SC_RETURN_VALUE;
    }

    long wx_GetNumberFromUser(const wxString& message, const wxString& prompt, const wxString& caption, long value)
    {
        return wxGetNumberFromUser(message, prompt, caption, value);
    }
    wxString wx_GetPasswordFromUser(const wxString& message, const wxString& caption, const wxString& default_value)
    {
        return wxGetPasswordFromUser(message, caption, default_value);
    }
    wxString wx_GetTextFromUser(const wxString& message, const wxString& caption, const wxString& default_value)
    {
        return wxGetTextFromUser(message, caption, default_value);
    }

    wxArrayString wx_GetFileFromUser(const wxString& message, const wxString& caption, const wxString& default_value,const wxString& wildcart,long style)
    {
        wxFileDialog* filedlg;
        filedlg = new wxFileDialog(nullptr,message,default_value,wxEmptyString,wildcart,style);
        wxArrayString ret;
        if(filedlg->ShowModal() == wxID_OK)
        {
            filedlg->GetPaths(ret);
        }
        return ret;
    }

    wxString wx_GetDirFromUser(const wxString& message, const wxString& default_path,long style)
    {
        return wxDirSelector(message,default_path,style,wxDefaultPosition,nullptr);
    }

    void bind_wx_util_dialogs(HSQUIRRELVM vm)
    {
        /** \ingroup sq_dialogs
         *### wxGetColourFromUser(value)
         *
         *  - __value__     A default value [wxColor]
         *
         *  This function displays an dialog  with the possibility to ask a color from the user
         *
         *  - __return__ The Value entered by the user or _value_
         */
        /** \ingroup sq_dialogs
         *### wxGetNumberFromUser(message,prompt,caption,value)
         *
         *  - __message__   Message to inform the user [wxString]
         *  - __prompt__    Prompt to inform the user [wxString]
         *  - __caption__   The caption for the Dialog [wxString]
         *  - __value__     A default value [int]
         *
         *  This function displays an dialog  with the possibility to ask a number from the user
         *
         *  - __return__ The Value entered by the user or _value_
         */

        /** \ingroup sq_dialogs
         *### wxGetPasswordFromUser(message,prompt,caption,value)
         *
         *  - __message__   Message to inform the user [wxString]
         *  - __caption__   The caption for the Dialog [wxString]
         *  - __value__     A default value [wxString]
         *
         *  This function displays an dialog  with the possibility to ask for a password
         *
         *  - __return__ The Value entered by the user or _value_
         */

        /** \ingroup sq_dialogs
         *### wxGetTextFromUser(key,default_value)
         *
         *  - __message__   Message to inform the user [wxString]
         *  - __caption__   The caption for the Dialog [wxString]
         *  - __value__     A default value [wxString]
         *
         *  This function displays an dialog  with the possibility to ask for a text from the user
         *
         *  - __return__ The Value entered by the user or _value_
         */

         /** \ingroup sq_dialogs
         *### wxGetFileFromUser(message, caption, default_value,wildcart,style)
         *
         *  - __message__        Message to inform the user [wxString]
         *  - __caption__        ... [wxString]
         *  - __default_value__  The Default file/path [wxString]
         *  - __wildcart__       The wildcart to filter the files [wxString]
         *  - __style__          The styles: wxFD_DEFAULT_STYLE, wxFD_OPEN, wxFD_SAVE, wxFD_OVERWRITE_PROMPT, wxFD_FILE_MUST_EXIST, wxFD_MULTIPLE, wxFD_CHANGE_DIR, wxFD_PREVIEW
         *
         *
         *  This function displays an dialog  with the possibility to ask for one or multiple files to be saved or loaded
         *
         *  - __return__ A wxArrayString with the given files (empty if the user canceled the dialog)
         */

         /** \ingroup sq_dialogs
         *### wxBell()
         *
         *  Ring the system Bell
         *
         *  - __return__
         */

        /** \ingroup sq_dialogs
         *### wxDirSelector(message,default_value,style)
         *
         *  - __message__        Message to inform the user [wxString]
         *  - __default_value__  The Default path [wxString]
         *  - __style__          The styles: wxDD_DEFAULT_STYLE, wxDD_DIR_MUST_EXIST, wxDD_CHANGE_DIR
         *
         *  This function displays an dialog  with the possibility to ask for a path
         *
         *  - __return__ A wxString with the given path ( a empty string if the user aborts the dialog)
         */

        BIND_INT_CONSTANT(wxDD_DEFAULT_STYLE);
        BIND_INT_CONSTANT(wxDD_DIR_MUST_EXIST);
        BIND_INT_CONSTANT(wxDD_CHANGE_DIR);

        // FileDialog styles
        BIND_INT_CONSTANT(wxFD_DEFAULT_STYLE);
        BIND_INT_CONSTANT(wxFD_OPEN);
        BIND_INT_CONSTANT(wxFD_SAVE);
        BIND_INT_CONSTANT(wxFD_OVERWRITE_PROMPT);
        BIND_INT_CONSTANT(wxFD_FILE_MUST_EXIST);
        BIND_INT_CONSTANT(wxFD_MULTIPLE);
        BIND_INT_CONSTANT(wxFD_CHANGE_DIR);
        BIND_INT_CONSTANT(wxFD_PREVIEW);


        Sqrat::RootTable()
        .SquirrelFunc(_SC("wxGetColourFromUser"),&wx_GetColourFromUser)
        .Func(_SC("wxGetNumberFromUser"),&wx_GetNumberFromUser)
        .Func(_SC("wxGetPasswordFromUser"),&wx_GetPasswordFromUser)
        .Func(_SC("wxGetTextFromUser"),&wx_GetTextFromUser)
        .Func(_SC("wxGetFileFromUser"),&wx_GetFileFromUser)
        .Func(_SC("wxBell"),&wxBell)
        .Func(_SC("wxDirSelector"),&wx_GetDirFromUser);

    }
}
