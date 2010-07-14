#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.07";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 606;
	static const long REVISION = 8089;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1237;
	#define RC_FILEVERSION 1,6,606,8089
	#define RC_FILEVERSION_STRING "1, 6, 606, 8089\0"
	static const char FULLVERSION_STRING[] = "1.6.606.8089";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 79;
	

}
#endif //VERSION_H
