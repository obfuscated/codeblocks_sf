
// Interface Dependencies ---------------------------------------------
#ifndef HooverDrawlet_h
#define HooverDrawlet_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// END Interface Dependencies -----------------------------------------

class HooverDrawlet
{
  public:
		HooverDrawlet();
		virtual ~HooverDrawlet();


		virtual bool Draw(wxDC &dc);
        virtual void UnDraw(wxDC &dc) = 0;

	private:
};

#endif



