#include "logger.h"
#include <manager.h>
#include <logmanager.h>


void LoggerSingleton::Log(wxString str)
{
    Manager::Get()->GetLogManager()->Log(str);
}
LoggerSingleton* LoggerSingleton::instanz = 0;

LoggerSingleton* LoggerSingleton::exemplar()
{
  static Waechter w;
  if( instanz == 0 )
    instanz = new LoggerSingleton();
  return instanz;
}


