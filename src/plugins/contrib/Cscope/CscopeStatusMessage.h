#ifndef CSCOPE_STATUS_MESSAGE
#define CSCOPE_STATUS_MESSAGE

#include <wx/string.h>

class CScopeStatusMessage
{
public:
    CScopeStatusMessage(const wxString &msg, int percent);
    CScopeStatusMessage(const CScopeStatusMessage &b);
    virtual ~CScopeStatusMessage();

    const wxString& GetText() const;
    const int& GetPercentage() const;
private:
    wxString m_text;
    int m_percentage;
};
#endif // CSCOPE_STATUS_MESSAGE
