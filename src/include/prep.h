#if ( !defined (PREP_H) && defined(__cplusplus) )
#define PREP_H

#ifndef wxMAJOR_VERSION
#include <wx/version.h>
#endif


/*  ---------------------------------------------------------------------------------------------------------
    Version<major, minor, revision>::eval
        Integer compile-time constant that represents  a major.minor.revision style version number.
        This is the most convenient and efficient way to check for API-version dependent features,
        as it can be done as easily as:  if(foobar_version >= Version<1,2>::eval)

    wxMinimumVersion<...>::eval, wxExactVersion<...>::eval
        Boolean values that are true if the compiled wxWidgets version is at least (exactly)
        the version given as parameters.

    Example:
        if(!wxMinimumVersion<2.8>::eval && (foo_version < Version<1.2>::eval))
            ErrorMessage("This feature is only supported under wxWidgets 2.8, or with Foo Component 1.2 or higher.");
*/
template <int major, int minor = 0, int revision = 0> struct Version { enum { eval = 1000*1000*major + 1000*minor + revision }; };
template <int major, int minor, int rel = 0> struct wxMinimumVersion { enum { eval = ((unsigned int)  Version<wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER>::eval >= (unsigned int) Version<major, minor, rel>::eval) }; };
template <int major, int minor, int rel = 0> struct wxExactVersion { enum { eval = ((unsigned int) Version<wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER>::eval == (unsigned int) Version<major, minor, rel>::eval) }; };



/*  ---------------------------------------------------------------------------------------------------------
    Assert a condition at compile time (as assert() does at runtime)
    Break compilation if the assertion fails.

    Example:
		CompileTimeAssertion<wxMinimumVersion<2,6>::eval>::Assert();

		This example code will break the build if you try to compile the code with wxWindows 2.4 (or any
		other version below 2.6).
		However, it will break the build in such a way that the problem is apparent in the error message,
		rather than throwing up 317 obscure errors about whatever undefined symbols and wrong types.
*/
template <bool b> struct CompileTimeAssertion{};
template<> struct CompileTimeAssertion<true> { static inline void Assert(){}; };



/*  ---------------------------------------------------------------------------------------------------------
    Conditional typedef, works the same way as the C++ ternary operator for lvalues does

    You want to do something like:
        typedef (condition ? true_type : false_type) my_type;

    This can for example be used to define a type as
        FILE under Unix / HANDLE under Windows
        int under wxWidgets 2.6 / size_t under wxWidgets 2.8
        STL iterator / wxContainer iterator / pointer ...
    and not having to worry about what it really is, without giving up type safety and without
    the nasty side effects that a #define might have.

	Example:
		typedef TernaryCondTypedef<wxMinimumVersion<2,5>::eval, wxTreeItemIdValue, long int>::eval tree_cookie_t;
*/
template <bool cond, class true_t, class false_t> struct TernaryCondTypedef { typedef true_t eval; };
template <class true_t, class false_t> struct TernaryCondTypedef<false, true_t, false_t> { typedef false_t eval; };


/*  ---------------------------------------------------------------------------------------------------------
    Size of an array (if the compiler can determine it at compile time)
    Write:
        int widths[] = {5, 3, 8};
        myListControl->SetWidths(widths, array_size(widths));
    instead of:
        int widths[] = {5, 3, 8};
        myListControl->SetWidths(widths, 4); // oh crap, why does this crash?
*/
template <typename T> unsigned int array_size(const T& array) { enum {result = sizeof(array) / sizeof(array[0])}; return result; };



/*  ---------------------------------------------------------------------------------------------------------
    Delete a pointer and (semi-atomically) set it to zero.
    In class destructors, please continue using the normal C++ delete operator (it is unnecessary overhead
    to set a pointer to zero in the destructor, as it can never be used again).
    In _all_ other cases, use Delete(), which prevents accidential double-deletes.
*/
template<typename T>inline void Delete(T*& p){delete p; p = 0;};
template<typename T>inline void DeleteArray(T*& p){delete[] p; p = 0;};



/*  ---------------------------------------------------------------------------------------------------------
    platform::id
        Value of type platform::identifier describing the target platform

    platform::windows, platform::macosx, platform::linux
    platform::freebsd, platform::netbsd, platform::openbsd
    platform::darwin,  platform::solaris, platform::unix
        Boolean value that evaluates to true if the target platform is the same as the variable's name, false otherwise.
        Using the platform booleans is equivalent to using platform::id, but results in nicer code.

    platform::unicode
        Boolean value that evaluates to true if this application was built in Unicode mode. Of course this does not
        tell anything about the host system's actual capabilities.

    platform::gtk
    platform::carbon
    platform::cocoa
        Boolean values showing the presence of very specific toolkits. Use only for workarounds to specific problems with these.

    platform::bits
        Size of pointer in bits as a measure of CPU architecture (32 or 64 bits).

    platform::gcc
        The gcc version number as Version<...> if gcc was used to build the program, zero otherwise.
*/
namespace platform
{
    enum identifier
    {
        platform_unknown,
        platform_windows,
        platform_linux,
        platform_freebsd,
        platform_netbsd,
        platform_openbsd,
        platform_darwin,
        platform_solaris,
        platform_macosx
    };

    // unfortunately we still need to use the preprocessor here...
    #if ( wxUSE_UNICODE )
    const bool unicode = true;
    #else
    const bool unicode = false;
    #endif

    #if   defined ( __WIN32__ )
    const identifier id = platform_windows;
    #elif defined ( __WXMAC__ )  || defined ( __WXCOCOA__ )
    const identifier id = platform_macosx;
    #elif defined ( __linux__ )  || defined ( LINUX )
    const identifier id = platform_linux;
    #elif defined ( FREEBSD )    || defined ( __FREEBSD__ )
    const identifier id = platform_freebsd;
    #elif defined ( NETBSD )     || defined ( __NETBSD__ )
    const identifier id = platform_netbsd;
    #elif defined ( OPENBSD )    || defined ( __OPENBSD__ )
    const identifier id = platform_openbsd;
    #elif defined ( DARWIN )     || defined ( __APPLE__ )
    const identifier id = platform_darwin;
    #elif defined(sun) || defined(__sun)
    const identifier id = platform_solaris;
    #else
    const identifier id = platform_unknown;
    #endif

    #if   defined ( __WXGTK__ )
    const bool gtk = true;
    #else
    const bool gtk = false;
    #endif

    #if   defined ( __WXMAC__ )
    const bool carbon = true;
    #else
    const bool carbon = false;
    #endif

    #if   defined ( __WXCOCOA__ )
    const bool cocoa = true;
    #else
    const bool cocoa = false;
    #endif

	#if defined ( linux )
		#undef linux
	#endif

	#if defined ( unix )
		#undef unix
	#endif

    const bool windows = (id == platform_windows);
    const bool macosx  = (id == platform_macosx);
    const bool linux   = (id == platform_linux);
    const bool freebsd = (id == platform_freebsd);
    const bool netbsd  = (id == platform_netbsd);
    const bool openbsd = (id == platform_openbsd);
    const bool darwin  = (id == platform_darwin);
    const bool solaris = (id == platform_solaris);
    const bool unix    = (linux | freebsd | netbsd | openbsd | darwin | solaris);

    const int bits = 8*sizeof(void*);


    #if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2)
    const int gcc = Version<__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__>::eval;
    #else
    const int gcc = 0;
    #endif
};



/*  ---------------------------------------------------------------------------------------------------------
    Nobody except Yiannis touches these. You don't need to know, you don't want to know.
*/
namespace sdk
{
    const int version = Version<1>::eval;
    const int buildsystem_version = Version<1>::eval;
    const int plugin_api_version = Version<1,11,10>::eval;
};



/*  ---------------------------------------------------------------------------------------------------------
    The compatibility namespace is intended for workarounds that try to cope with incompatibilities in different
    wxWidgets versions. Since these often involve missing functions or constants, #ifdef is explicitely allowed
    and not frowned upon in this namespace.

    wxHideReadonly
        use in file selector dialog to keep wxWidgets 2.4/2.6 users happy (this flag is important for normal operation!),
        without breaking the 2.8 build.
*/
namespace compatibility
{
    #if defined(WXWIN_COMPATIBILITY_2_4) && defined(wxHIDE_READONLY)
        const int wxHideReadonly = wxHIDE_READONLY;
    #else
        const int wxHideReadonly = 0;
    #endif
}



/*  ---------------------------------------------------------------------------------------------------------
    Utility function for an incrementing (unique per instance) unsigned integer ID.
        - ID is unsigned, starting at zero (best choice for many cases, e.g. for use as array index)
        - if it _must_ start with 1 for some odd reason, simply add one extra call to GetID()<YourClass>
        - use GetID() for an application-wide unique integer ID (alias for GetID<void>())
        - use GetID<YourClass>() for your own private class-internal ID
        - use GetID<SomeStruct>() for your own private cross-class shareable ID

        ((  This is implementation is more general and uses one less temporary copy
            than the implementation found in several places of the SDK.
            NOTE: remove this paragraph once the SDK has been updated  ))

    Example:

        struct foo; // used for sharing an ID between A and B

        class A
        {
        public:
            unsigned int X(){return GetID(); };         // application-global
            unsigned int Y(){return GetID<A>(); };      // private for A
            unsigned int Z(){return GetID<foo>(); };    // shared with B

        };

        class B
        {
        public:
            unsigned int Y(){return GetID<B>(); };      // private for B
            unsigned int Z(){return GetID<foo>(); };    // shared with A
        };

        In this example, A::X() will return a counter that is globally unique througout the lifetime of the application.
        A::Y() and B::Y() will return counters that increment independently for A and B. In other words,
        B does not know about A's counter, nor can it influence it.
        A::Z() and B::Z() will return a shared counter that increments if either A or B is asked to return a value.
*/

class ID
{
    unsigned int value;

    ID(unsigned int in) : value(in) {};

    template<typename> friend ID GetID();
    friend ID ConstructID(unsigned int);

public:

    ID() : value ((unsigned) -1) {};

    operator unsigned int() const { return value; };
    operator void*() const { return (void*) value; };

    bool Valid() const { return value != ((unsigned) -1); };
    bool operator!() const { return !Valid(); };
};


template<typename whatever> inline ID GetID()
{
    static unsigned int id = (unsigned int) -1;
    return ID(++id);
};

inline ID GetID() { return GetID<void>(); };
inline ID ConstructID(unsigned int i) { return ID(i); };


#endif
