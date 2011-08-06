/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBSTATUSBAR_H
#define CBSTATUSBAR_H

#include <vector>
#include <wx/statusbr.h>
#include "settings.h"


class cbPlugin;


/** @brief A custom status bar which can contain controls, icons...
  */
class DLLIMPORT cbStatusBar : public wxStatusBar
{
private:
    friend class MainFrame; // gets initialized from mainframe
    cbStatusBar(wxWindow *parent,  wxWindowID id, long style, const wxString& name); // this will be used from friend mainframe
    cbStatusBar(){};
    cbStatusBar ( const cbStatusBar& );
    ~cbStatusBar();
//private:
//    virtual bool GetFieldRect(int i, wxRect& rect) const
//    virtual wxString GetStatusText(int i = 0) const
//    void PopStatusText(int field = 0)
//    void PushStatusText(const wxString& string, int field = 0)
//    virtual void SetFieldsCount(int number = 1, int* widths = NULL)
//    virtual void SetStatusText(const wxString& text, int i = 0)
//    virtual void SetStatusStyles(int n, int *styles)

public:
    /** Sets the text for one field.
        @param text  The text to be set.
        @param plugin  The pugin which wants to set its status text
      */
    virtual void SetStatusText(const wxString& text, cbPlugin *plugin);

    /** Returns the string associated with a status bar field.
        @param  plugin  The plugin which wants to get its status text
        @return The  status field string if the field id is valid, otherwise the empty string.
      */
    virtual wxString GetStatusText(cbPlugin *plugin) const;

    /** Returns the size and position of a field's internal bounding rectangle.
        @param  plugin  The plugin which wants to know its status-field size.
        @param rect  The rectangle values are placed in this variable.
        @return true if the field id is valid, false otherwise.
      */
    virtual bool GetFieldRect(cbPlugin *plugin, wxRect& rect) const;

private:
    void AdjustFieldsSize();
    void PushStatusText(const wxString& text,cbPlugin *plugin){};
    void PopStatusText(cbPlugin *plugin){};

public:
    // functions used for the mainframe:
    virtual void SetStatusWidths(int n, const int* widths);
private:
    std::vector<int> m_mainswidths;

public:
    /** Add a new field, which contains a control, to the status bar.
        @param   plugin The plugin which wants to add a new statusfield
        @param   ctrl   Pointer to the control to show on the new field.
        @param   width  Width of the new field. See wxStatusBar::SetStatusWidths for the meaning of this parameter.
      **/
    void AddField(cbPlugin *plugin, wxWindow *ctrl, int width );

    /** Add a new field to the status bar. The field contains no control.
        @see cbStatusBar::AddField(cbPlugin *plugin, wxWindow *ctrl, int width) for details.
      **/
    void AddField(cbPlugin *plugin, int width);

    /** Remove a field from the status bar.
        If a control is associated wit this filed, RemoveField will destroy it.
        @param id   The plugin which wants to remove its field
      **/
    void RemoveField(cbPlugin *plugin);


private: // event handlers
    void OnSize(wxSizeEvent& event);
    void UpdateWidths();

private:

    class cbStatusBarElement{
        public:
            int width;
            wxWindow *control;
            cbPlugin *plugin;
    };
    typedef std::vector<cbStatusBarElement> ElementVector;
    ElementVector m_elements;

private:
    int GetFieldNumberOfPlugin(cbPlugin *plugin)const;

    DECLARE_EVENT_TABLE()
};

#endif //CBSTATUSBAR_H
