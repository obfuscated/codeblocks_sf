#ifndef __LOGGER_H__
#define __LOGGER_H__

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class LoggerSingleton
{
    public:
        void Log(wxString);

    public:
        static LoggerSingleton* exemplar();

    private:
        static LoggerSingleton *instanz;
        LoggerSingleton() {}
        LoggerSingleton( const LoggerSingleton& );

        ~LoggerSingleton() {}

        class Waechter {
            public: ~Waechter()
            {
                if( LoggerSingleton::instanz != 0 )
                    delete LoggerSingleton::instanz;
            }
        };
        friend class Waechter;
};



#endif //#ifndef __LOGGER_H__

