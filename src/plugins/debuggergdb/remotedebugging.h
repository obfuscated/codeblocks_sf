#ifndef REMOTEDEBUGGING_H
#define REMOTEDEBUGGING_H

#include <map>
#include <wx/string.h>

class ProjectBuildTarget;

// per-target remote debugging support
struct RemoteDebugging
{
	enum ConnectionType
	{
		TCP = 0,
		UDP,
		Serial
	};

	bool IsOk()
	{
		return connType == Serial
				? (!serialPort.IsEmpty() && !serialBaud.IsEmpty())
				: (!ip.IsEmpty() && !ipPort.IsEmpty());
	}
	
	ConnectionType connType;
	wxString serialPort;
	wxString serialBaud;
	wxString ip;
	wxString ipPort;
	wxString additionalCmds;
};

typedef std::map<ProjectBuildTarget*, RemoteDebugging> RemoteDebuggingMap;


#endif // REMOTEDEBUGGING_H
