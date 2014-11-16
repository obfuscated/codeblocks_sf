/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef HEADERFIXUP_H_INCLUDED
#define HEADERFIXUP_H_INCLUDED

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cbplugin.h>
#include <cbproject.h>
#include <configurationpanel.h>

class wxWindow;

class HeaderFixup : public cbToolPlugin
{
public:

  HeaderFixup();
  virtual ~HeaderFixup();
  virtual int GetConfigurationPriority() const
  { return 50; }
  virtual int GetConfigurationGroup() const
  { return cgUnknown; }
  virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
  virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* /*parent*/, cbProject* /*project*/)
  { return 0; }
  virtual int Execute();

protected:

  virtual void OnAttach();
  virtual void OnRelease(bool appShutDown);
};

#endif
