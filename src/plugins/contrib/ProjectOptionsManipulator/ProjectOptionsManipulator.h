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

  void ProcessCompilerOptions(cbProject* prj, const wxString& opt, wxArrayString& result);
  void ProcessLinkerOptions  (cbProject* prj, const wxString& opt, wxArrayString& result);

  void ProcessCompilerPaths(cbProject* prj, const wxString& path, wxArrayString& result);
  void ProcessLinkerPaths  (cbProject* prj, const wxString& path, wxArrayString& result);

  ProjectOptionsManipulatorDlg* m_Dlg;

  DECLARE_EVENT_TABLE()
};

#endif // ProjectOptionsManipulator_H
