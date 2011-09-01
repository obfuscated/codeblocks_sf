#ifndef EDITPROPERTIES_H
#define EDITPROPERTIES_H


class ScbEditor;
class wxScrollingDialog;
//----------------------------------------------------------------------------
// EditProperties
//----------------------------------------------------------------------------
class EditProperties: public wxScrollingDialog {
//----------------------------------------------------------------------------

public:

    // constructor
    EditProperties (wxWindow* pParent, ScbEditor* pEditor, long style = 0);

private:
    void InitEditProperties (wxWindow* pParent, ScbEditor* pEditor, long style);

};

#endif //EDITPROPERTIES_H
