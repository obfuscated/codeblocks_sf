#ifndef REDHATCHDRAWLET_H
#define REDHATCHDRAWLET_H

#include "HooverDrawlet.h"


class RedHatchDrawlet : public HooverDrawlet
{
    public:
        RedHatchDrawlet(wxRect rect);
        virtual ~RedHatchDrawlet();

        bool Draw(wxDC &dc);
        void UnDraw(wxDC &dc);
    protected:
    private:
        wxRect m_rect;
};

#endif // REDHATCHDRAWLET_H
