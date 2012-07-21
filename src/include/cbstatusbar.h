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

    // this will be used from friend mainframe
    cbStatusBar(wxWindow* parent,  wxWindowID id, long style, const wxString& name);
    cbStatusBar() { ; };
    cbStatusBar ( const cbStatusBar& );
    virtual ~cbStatusBar();

public:
    /** Sets the text for one field.
        @param text  The text to be set.
        @param plugin  The pugin which wants to set its status text
      */
    virtual void SetStatusTextByPlugin(const wxString& text, cbPlugin* plugin);

    /** Returns the string associated with a status bar field.
        @param  plugin  The plugin which wants to get its status text
        @return The  status field string if the field id is valid, otherwise the empty string.
      */
    virtual wxString GetStatusTextByPlugin(cbPlugin* plugin) const;

    /** Returns the size and position of a field's internal bounding rectangle.
        @param  plugin  The plugin which wants to know its status-field size.
        @param rect  The rectangle values are placed in this variable.
        @return true if the field id is valid, false otherwise.
      */
    virtual bool GetFieldRectByPlugin(cbPlugin *plugin, wxRect& rect) const;

    // functions used for the mainframe:
    virtual void SetStatusWidths(int n, const int* widths);

    /** Add a new field, which contains a control, to the status bar.
        @param   plugin The plugin which wants to add a new statusfield
        @param   ctrl   Pointer to the control to show on the new field.
        @param   width  Width of the new field. See wxStatusBar::SetStatusWidths for the meaning of this parameter.
      **/
    void AddField(cbPlugin* plugin, wxWindow* ctrl, int width);

    /** Add a new field to the status bar. The field contains no control.
        @see cbStatusBar::AddField(cbPlugin *plugin, wxWindow *ctrl, int width) for details.
      **/
    void AddField(cbPlugin* plugin, int width);

    /** Remove a field from the status bar.
        If a control is associated wit this filed, RemoveField will destroy it.
        @param id   The plugin which wants to remove its field
      **/
    void RemoveField(cbPlugin* plugin);

private:
    // event handlers
    void OnSize(wxSizeEvent& event);
    void UpdateWidths();

    void AdjustFieldsSize();
    int  GetFieldNumberOfPlugin(cbPlugin* plugin) const;

    void PushStatusText(const wxString& text,cbPlugin *plugin) { ; };
    void PopStatusText(cbPlugin* plugin)                       { ; };

    struct cbStatusBarElement
    {
        int       width;
        wxWindow *control;
        cbPlugin *plugin;
    };
    typedef std::vector<cbStatusBarElement> ElementVector;
    ElementVector                           m_Elements;
    std::vector<int>                        m_MainWidths;

    DECLARE_EVENT_TABLE()
};

#endif //CBSTATUSBAR_H
