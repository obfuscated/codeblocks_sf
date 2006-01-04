#pragma once

#include <wx/wx.h>
#include <wx/dnd.h>
#include <iostream>
using namespace std;

template <class T>
class CTextDropTarget : public wxTextDropTarget
{

private:
	typedef bool (T::*pt2Func)(wxCoord, wxCoord, const wxString&);
	T* m_pParent;
	pt2Func m_pt2CallbackFunc;

public:
	CTextDropTarget(T* pParent, pt2Func pt2CallbackFunc);
	virtual ~CTextDropTarget(void);

	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
};

template <class T>
CTextDropTarget<T>::CTextDropTarget(T* pParent, pt2Func pt2CallbackFunc)
: m_pParent(pParent)
, m_pt2CallbackFunc(pt2CallbackFunc)
{
}

template <class T>
CTextDropTarget<T>::~CTextDropTarget(void)
{
}

template <class T>
bool CTextDropTarget<T>::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
	return (m_pParent->*m_pt2CallbackFunc)(x, y, data);
}
