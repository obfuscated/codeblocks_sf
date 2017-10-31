#ifndef REDLINEDRAWLET_H
#define REDLINEDRAWLET_H

#include "HooverDrawlet.h"


class RedLineDrawlet : public HooverDrawlet
{
    public:
        RedLineDrawlet(wxPoint pos, wxCoord length, bool vertical = true);
        virtual ~RedLineDrawlet();

        bool Draw(wxDC &dc);
        void UnDraw(wxDC &dc);
    protected:
    private:
        wxPoint m_pos;
        wxCoord m_length;
        bool m_vertical;
};

#endif // REDLINEDRAWLET_H
