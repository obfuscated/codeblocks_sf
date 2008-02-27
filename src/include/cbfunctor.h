/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CB_FUNCTOR_H
#define CB_FUNCTOR_H

/** Base abstract functor class. All functors must extend this interface. */
class IFunctorBase
{
	public:
		virtual ~IFunctorBase(){}
		virtual void* GetThis() = 0;
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
		virtual void Call(EventType& event) { if (m_pThis) (m_pThis->*m_pMember)(event); }
};

#endif // CB_FUNCTOR_H
