


/* C::B begin */
#ifndef wxOVERRIDE
    #define wxOVERRIDE override
#endif // wxOVERRIDE
#ifndef wxFALLTHROUGH
    #define wxFALLTHROUGH
#endif // wxFALLTHROUGH
/* C::B end */

wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
wxColour wxColourFromCD(const ColourDesired& ca);

