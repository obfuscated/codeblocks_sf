#ifndef sanitycheck_h
#define sanitycheck_h

///////////////////////////////////////////////////////////////////////////
//                       SANITY CHECK FOR C++ CLASSES                    //
//            Builds robust code and makes memory debugging easier       //
// (c) 2005 Ricardo Garcia                                               //
// This file can be distributed freely, as long as this copyright notice //
// remains intact.                                                       //
// Any modifications must be explicitly stated.                          //
///////////////////////////////////////////////////////////////////////////

// Instructions:
// Add the DECLARE_SANITY_CHECK macro after the last line of your class
// definition. This code, being protected, is inheritable so you just
// need to include it in your base classes declarations.
//
// The SC_CONSTRUCTOR_BEGIN macro must be the FIRST thing in your class'
// constructors.
//
// Use SANITY_CHECK(x) Macro for this purpose; If the macro doesn't consider
// That it's operating on a safe memory zone, then it returns value x. You
// can invoke the Macro as many times as you want in one function.
// In any case we're operating in a forbidden memory zone (dereferenced pointer),
// the SANITY_CHECK macro will operate as a SEGFAULT lightning rod.
// For debugging, you can just check the call stack and find out what was the
// problem.
//
// The SC_DESTRUCTOR_BEGIN macro signals local shutdown. Any functions you call
// from this point must _NOT_ call the sanity check macros! They will fail by
// default! Use SANITY_CHECK_ADVANCED(x) instead.
// The SANITY_CHECK_NULL(x) macro only checks that self==NULL and is safe to
// use in constructor and destructor related functions.
//
// The SC_DESTRUCTOR_END clears the m_sanitycheck_self pointer and is the last
// thing that should be executed in your class' code.
// Needless to say, static class functions should not use this code.

#define DECLARE_SANITY_CHECK                                            \
    protected:                                                          \
        inline bool sanity_check()                                      \
        {                                                               \
            if(!this) return false;                                     \
            if((void*)this!=m_sanitycheck_self)                         \
            return false;                                               \
            if(m_sanitycheck_shutdown)                                  \
                return false;                                           \
            return true;                                                \
        }                                                               \
        inline bool sanity_check_advanced()                             \
        {                                                               \
            if(!this) return false;                                     \
            if((void*)this!=m_sanitycheck_self)                         \
                return false;                                           \
            return true;                                                \
        }                                                               \
        bool m_sanitycheck_shutdown;                                    \
        void *m_sanitycheck_self;


#define SC_CONSTRUCTOR_BEGIN m_sanitycheck_shutdown = false;m_sanitycheck_self=this;
#define SC_DESTRUCTOR_BEGIN m_sanitycheck_shutdown = true;
#define SC_DESTRUCTOR_END m_sanitycheck_self=0L;
#define SANITY_CHECK(x) if(!sanity_check()) return x;
#define SANITY_CHECK_ADVANCED(x) if(!sanity_check_advanced()) return x;
#define SANITY_CHECK_NULL(x) if(!this) return x;

#endif
