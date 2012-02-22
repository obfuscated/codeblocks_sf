
#ifndef _MP_MATHPLOTEXTRA_H_
#define _MP_MATHPLOTEXTRA_H_

#include    <mathplot.h>



//-----------------------------------------------------------------------------
// mpText - provided by Val Greene
//-----------------------------------------------------------------------------

class mpMarker : public mpLayer
{
public:

    mpMarker(wxString name = wxT("[M]"), double atX = 0.0, double atY = 0.0);

            void SetPos(double atX, double atY) {mX = atX, mY = atY; };

    virtual void Plot(wxDC & dc, mpWindow & w);

    virtual bool HasBBox() { return FALSE; }

protected:

    double  mX, mY;

    DECLARE_CLASS(mpText)
};


/*@}*/

#endif // _MP_MATHPLOT_H_
