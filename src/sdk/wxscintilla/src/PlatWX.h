#ifndef PLATWX_H
#define PLATWX_H




wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
wxColour wxColourFromCD(ColourDesired cd);

#endif
