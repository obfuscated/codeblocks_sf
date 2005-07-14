#ifndef CUSTOMVARS_H
#define CUSTOMVARS_H

#include "settings.h"

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
		CustomVars();
		~CustomVars();
		
		void Load(const wxString& configpath);
		void Save(const wxString& configpath);
		void Clear();
		
		void Add(const wxString& name, const wxString& value);
		Var* GetVarByName(const wxString& name);
		const VarsArray& GetVars(){ return m_Vars; }
		bool DeleteVar(const wxString& name);
		bool DeleteVar(Var* var);
		
		void ApplyVarsToEnvironment();
		
		bool GetModified(){ return m_Modified; }
		void SetModified(bool modified = true){ m_Modified = modified; }
	protected:
		Var* VarExists(const wxString& name);
		void DoAdd(const wxString& name, const wxString& value, bool builtin);
		void DoAdd(const Var& newvar);
		bool DoDeleteVar(Var* var, bool deleteIfBuiltin = false);
	private:
		bool m_Modified;
		VarsArray m_Vars;
};

#endif // CUSTOMVARS_H

