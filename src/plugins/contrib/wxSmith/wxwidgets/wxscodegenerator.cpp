#include "wxscodegenerator.h"

wxsCodeGenerator::wxsCodeGenerator(): m_Context(0)
{
}

wxsCodeGenerator::~wxsCodeGenerator()
{
    m_Context = 0;
}

void wxsCodeGenerator::BuildCode(wxsCoderContext* Context)
{
    wxsCoderContext* Store = m_Context;
    m_Context = Context;
    long FlagsStore = Context->m_Flags;

    OnUpdateFlags(Context->m_Flags);
    OnBuildCreatingCode();
    OnBuildHeadersCode();
    OnBuildDeclarationsCode();
    OnBuildEventsConnectingCode();
    OnBuildIdCode();
    OnBuildXRCFetchingCode();

    Context->m_Flags = FlagsStore;
    m_Context = Store;
}

