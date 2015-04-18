/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ProjectOptionsManipulator_H
#define ProjectOptionsManipulator_H

#include <wx/string.h>

#include <cbplugin.h> // the base class we 're inheriting

class ProjectOptionsManipulatorDlg;

class ProjectOptionsManipulator : public cbToolPlugin
{
public:
  ProjectOptionsManipulator();
  virtual ~ProjectOptionsManipulator();

  void OnAttach();
  void OnRelease(bool appShutDown);

  int  Execute();

private:
  bool OperateWorkspace(wxArrayString& result);
  bool OperateProject(size_t prj_idx, wxArrayString& result);
  bool OperateProject(cbProject* prj, wxArrayString& result);

  void ProcessFiles(cbProject* prj, wxArrayString& result);

  void ProcessCompilerOptions   (cbProject* prj, const wxString& opt, const wxString& opt_new, wxArrayString& result);
  void ProcessLinkerOptions     (cbProject* prj, const wxString& opt, const wxString& opt_new, wxArrayString& result);
  void ProcessResCompilerOptions(cbProject* prj, const wxString& opt, const wxString& opt_new, wxArrayString& result);

  void ProcessCompilerPaths(cbProject* prj, const wxString& path, const wxString& path_new, wxArrayString& result);
  void ProcessLinkerPaths  (cbProject* prj, const wxString& path, const wxString& path_new, wxArrayString& result);
  void ProcessResCompPaths (cbProject* prj, const wxString& path, const wxString& path_new, wxArrayString& result);

  void ProcessLinkerLibs(cbProject* prj, const wxString& lib, const wxString& lib_new, wxArrayString& result);

  void ProcessCustomVars(cbProject* prj, const wxString& var, const wxString& value, wxArrayString& result);

  bool HasOption(const wxArrayString& opt_array, const wxString& opt);
  bool HasOption(const wxArrayString& opt_array, const wxString& opt, wxString& full_opt);

  wxString ManipulateOption(wxString full_opt, const wxString& src, const wxString& dst);

  ProjectOptionsManipulatorDlg* m_Dlg;

  DECLARE_EVENT_TABLE()
};

#endif // ProjectOptionsManipulator_H
