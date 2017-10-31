/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CB_FUNCTOR_H
#define CB_FUNCTOR_H

//uncomment the below line if you want to do the performance measure of the event processing
//#define PPRCESS_EVENT_PERFORMANCE_MEASURE

#ifdef PPRCESS_EVENT_PERFORMANCE_MEASURE
#include <typeinfo> // typeid()
#endif // PPRCESS_EVENT_PERFORMANCE_MEASURE

/** Base abstract functor class. All functors must extend this interface. */
class IFunctorBase
{
	public:
		virtual ~IFunctorBase(){}
		virtual void* GetThis() = 0;

#ifdef PPRCESS_EVENT_PERFORMANCE_MEASURE
		// return the function name, this is used for Functor performance measure
		virtual const char* GetTypeName() = 0;
#endif // PPRCESS_EVENT_PERFORMANCE_MEASURE
};

/** Base abstract event functor class. All event functors must extend this interface.*/
template<typename EventType> class IEventFunctorBase : public IFunctorBase
{
	public:
		virtual void Call(EventType& event) = 0;
};

/** Event functor class. */
template<class ClassType, typename EventType> class cbEventFunctor : public IEventFunctorBase<EventType>
{
	private:
		typedef void (ClassType::*Member)(EventType&);
		ClassType* m_pThis;
		Member m_pMember;
	public:
		cbEventFunctor(ClassType* this_, Member member) : m_pThis(this_), m_pMember(member) {}
		cbEventFunctor(const cbEventFunctor<ClassType, EventType>& rhs) : m_pThis(rhs.m_pThis), m_pMember(rhs.m_pMember) {}
		virtual void* GetThis() { return m_pThis; }
		// usually the m_pThis is a pointer the instance of a specified class of ClassType
		// the m_pMember is a member function of the ClassType, so just call this member function
		virtual void Call(EventType& event) { if (m_pThis) (m_pThis->*m_pMember)(event); }

#ifdef PPRCESS_EVENT_PERFORMANCE_MEASURE
		// show the name by typeid operator
		virtual const char* GetTypeName(){return typeid(m_pMember).name();}
#endif // PPRCESS_EVENT_PERFORMANCE_MEASURE
};

#endif // CB_FUNCTOR_H
