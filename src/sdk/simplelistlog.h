#ifndef SIMPLELISTLOG_H
#define SIMPLELISTLOG_H

#include "settings.h"
#include "messagelog.h" // inheriting class's header file

class wxListCtrl;
class wxArrayString;

/*
 * No description
 */
class DLLIMPORT SimpleListLog : public MessageLog
{
	public:
		// class constructor
		SimpleListLog(bool fixedPitchFont = false);
		SimpleListLog(int numCols, int widths[], const wxArrayString& titles, bool fixedPitchFont = false);
		// class destructor
		~SimpleListLog();

		virtual void AddLog(const wxString& msg, bool addNewLine = true);
		wxListCtrl* GetListControl(){ return m_pList; }

		virtual void ResetLogFont();

		void SetColumns(int numCols, int widths[], const wxArrayString& titles);
		void AddLog(const wxArrayString& values);
		void Clear();
    protected:
        wxListCtrl* m_pList;
        bool m_UseFixedPitchFont;
    private:
		void Init(bool fixedPitchFont);
};

#endif // SIMPLELISTLOG_H
