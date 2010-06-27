#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "27";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.06";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 551;
	static const long REVISION = 7781;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1175;
	#define RC_FILEVERSION 1,6,551,7781
	#define RC_FILEVERSION_STRING "1, 6, 551, 7781\0"
	static const char FULLVERSION_STRING[] = "1.6.551.7781";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 24;
	

}
#endif //VERSION_H
