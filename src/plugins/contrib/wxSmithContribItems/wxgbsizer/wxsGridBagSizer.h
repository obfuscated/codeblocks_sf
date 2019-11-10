/* wxGridBagSizer implementation for wxSmith
 * Created by Stéphane Coquelin (sun.wu.kong@free.fr)
 *
 * Most of this file consist on copy / paste and adapt to fit to wxGridBagSizer.
 * I have used wxssizer.h and wxsflexgridsizer.h to create this file.
 *
 * Known problems :
 *    - only test on windows
 *    - I could have done a better work but I'm not the best in C/C++
 **************************************************************************************************/
#ifndef WXSGRIDBAGSIZER_H
#define WXSGRIDBAGSIZER_H

#include <wx/gbsizer.h>
#include "wxwidgets/wxssizer.h"

class wxsGridBagSizerExtra: public wxsSizerExtra
{
	public:
		long colspan;
		long rowspan;
		long col;
		long row;
		wxsGridBagSizerExtra();
		wxString AllParamsCode(wxsCoderContext* Ctx);

	protected:
		virtual void OnEnumProperties(long Flags);
};

class wxsGridBagSizer: public wxsSizer
{
	public:
		wxsGridBagSizer(wxsItemResData* Data);

	private:
		virtual void OnBuildCreatingCode();
		virtual wxsPropertyContainer* OnBuildExtra();
		virtual wxGridBagSizer* OnBuildSizerPreview(wxWindow* Parent);
		virtual void OnBuildSizerCreatingCode();
		virtual void OnEnumSizerProperties(long Flags);
		virtual bool OnIsPointer() { return true; }
		wxObject* OnBuildPreview(wxWindow* Parent,long Flags);

		wxsDimensionData VGap;
		wxsDimensionData HGap;
		wxString GrowableRows;
		wxString GrowableCols;
};

#endif // WXSGRIDBAGSIZER_H
