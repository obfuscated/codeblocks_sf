#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2012";
	static const char UBUNTU_VERSION_STYLE[] = "12.11";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 7;
	static const long BUILD = 663;
	static const long REVISION = 8397;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1332;
	#define RC_FILEVERSION 1,7,663,8397
	#define RC_FILEVERSION_STRING "1, 7, 663, 8397\0"
	static const char FULLVERSION_STRING[] = "1.7.663.8397";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 36;
	

}
#endif //VERSION_H
