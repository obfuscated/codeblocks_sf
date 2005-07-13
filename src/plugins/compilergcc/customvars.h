#ifndef CUSTOMVARS_H
#define CUSTOMVARS_H

#include <settings.h>

// forward decls
class CompilerGCC;

struct Var
{
	wxString name;
	wxString value;
	bool builtin;
};
WX_DECLARE_OBJARRAY(Var, VarsArray);

class CustomVars
{
	public:
		CustomVars(CompilerGCC* compiler);
		~CustomVars();
		
		void Load();
		void Save();
		void Clear();
		
		void Add(const wxString& name, const wxString& value);
		Var* GetVarByName(const wxString& name);
		const VarsArray& GetVars(){ return m_Vars; }
		bool DeleteVar(const wxString& name);
		bool DeleteVar(Var* var);
		
		void ApplyVarsToEnvironment();
	protected:
		Var* VarExists(const wxString& name);
		void DoAddDefaults();
		void DoAdd(const wxString& name, const wxString& value, bool builtin);
		void DoAdd(const Var& newvar);
		bool DoDeleteVar(Var* var, bool deleteIfBuiltin = false);
	private:
        CompilerGCC* m_pCompiler;
		VarsArray m_Vars;
};

#endif // CUSTOMVARS_H

