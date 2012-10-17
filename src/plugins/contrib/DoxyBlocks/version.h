#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "14";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2012";
	static const char UBUNTU_VERSION_STYLE[] = "12.10";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 7;
	static const long BUILD = 659;
	static const long REVISION = 8370;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1328;
	#define RC_FILEVERSION 1,7,659,8370
	#define RC_FILEVERSION_STRING "1, 7, 659, 8370\0"
	static const char FULLVERSION_STRING[] = "1.7.659.8370";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 32;
	

}
#endif //VERSION_H
