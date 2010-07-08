#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "05";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.07";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 596;
	static const long REVISION = 8034;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1228;
	#define RC_FILEVERSION 1,6,596,8034
	#define RC_FILEVERSION_STRING "1, 6, 596, 8034\0"
	static const char FULLVERSION_STRING[] = "1.6.596.8034";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 69;
	

}
#endif //VERSION_H
