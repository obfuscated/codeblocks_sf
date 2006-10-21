#ifndef SYMTAB_H
#define SYMTAB_H

#include "cbplugin.h" // the base class we 're inheriting

class cbConfigurationPanel;
class SymTabConfigDlg;
class SymTabExecDlg;
class wxWindow;

class SymTab : public cbToolPlugin
{
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
public:
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
           SymTab();
  virtual ~SymTab();

  int  Execute();

  void OnAttach(); // fires when the plugin is attached to the application
  void OnRelease(bool appShutDown); // fires when the plugin is released from the application

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
private:
/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- */
  SymTabConfigDlg* CfgDlg;
  SymTabExecDlg*   ExeDlg;
};

#endif // SYMTAB_H

