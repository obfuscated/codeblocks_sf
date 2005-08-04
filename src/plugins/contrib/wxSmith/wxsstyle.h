#ifndef __WXSSTYLE_H
#define __WXSSTYLE_H

/** Structure describing one widget's stule */
struct wxsStyle
{
    const char* Name;
    unsigned int Value;
};

/******************************************************************************/
/* Usefull defines used while creating set of widget's styles                 */
/******************************************************************************/


/** Declaring list in header file */
#define WXS_ST_DECLARE(name)                      \
    extern wxsStyle* name;


/** Beginning definition of array (source file) */
#define WXS_ST_BEGIN(name)                      \
    static wxsStyle __##name##__[] = {
    
/** Adding new style into list */
#define WXS_ST(name)                            \
    { #name, name },
    
/** Beginning new  category */    
#define WXS_ST_CATEGORY(name)                   \
    { name, ((unsigned int)-1) },
    
/** Ending creation of list */    
#define WXS_ST_END(name)                        \
    { NULL, 0 } };                              \
    wxsStyle* name = __##name##__;
    
/** adding all default window's style */
#define WXS_ST_DEFAULTS()                       \
    WXS_ST_CATEGORY("wxWindow")                 \
    WXS_ST(wxSIMPLE_BORDER)                     \
    WXS_ST(wxDOUBLE_BORDER)                     \
    WXS_ST(wxSUNKEN_BORDER)                     \
    WXS_ST(wxRAISED_BORDER)                     \
    WXS_ST(wxSTATIC_BORDER)                     \
    WXS_ST(wxNO_BORDER)                         \
    WXS_ST(wxTRANSPARENT_WINDOW)                \
    WXS_ST(wxTAB_TRAVERSAL)                     \
    WXS_ST(wxWANTS_CHARS)                       \
    WXS_ST(wxNO_FULL_REPAINT_ON_RESIZE)         \
    WXS_ST(wxVSCROLL)                           \
    WXS_ST(wxHSCROLL)                           \
    WXS_ST(wxALWAYS_SHOW_SB)                    \
    WXS_ST(wxCLIP_CHILDREN)                     \
    WXS_ST(wxFULL_REPAINT_ON_RESIZE)
    

#endif
