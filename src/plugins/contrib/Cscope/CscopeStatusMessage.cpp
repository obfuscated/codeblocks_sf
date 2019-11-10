#include "CscopeStatusMessage.h"
CScopeStatusMessage::CScopeStatusMessage(const wxString &txt, int percent):
    m_text(txt),
    m_percentage(percent)
{}
CScopeStatusMessage::CScopeStatusMessage(const CScopeStatusMessage &b):
m_text(b.GetText()),
m_percentage(b.GetPercentage())
{
}

CScopeStatusMessage::~CScopeStatusMessage()
{
}

const wxString& CScopeStatusMessage::GetText() const
{
	return m_text;
}

const int& CScopeStatusMessage::GetPercentage() const
{
	return m_percentage;
}
