//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================
//
//	dice platform abstraction layer
//

#pragma once

#include "../../interface/tcat.h"
#include "../../interface/tcat_dice_defaults.h"
#include "../interface/tcat_dice.h"
#include "tcat_exception.h"
#include <sstream>
#include <deque>
#include <algorithm>


#define kTCAT_DICE_OHCI_RETRY_TIMEOUT	1000
#define kTCAT_DICE_OHCI_RETRY_DELAY		25


///////////////////////////////////////////////////////////////////////////////
//
//	debugging
//
///////////////////////////////////////////////////////////////////////////////

void pal_Debugger();

#ifdef WIN32
	#ifdef TCAT_DEBUG
		#include <intrin.h>
		#define	PAL_DEBUG_BREAK __debugbreak();
	#else
		#define	PAL_DEBUG_BREAK
	#endif // TCAT_DEBUG
#endif //WIN32

#ifdef __MACH__
	#define PAL_DEBUG_BREAK pal_Debugger();
#endif // __MACH__




namespace tcat
{
///////////////////////////////////////////////////////////////////////////////
//
//	to_string
//
///////////////////////////////////////////////////////////////////////////////
//\cond
template <class from_t> std::string to_string(const from_t& t) 
{ 
	std::ostringstream os; 
	os << t;
	return os.str();
}



///////////////////////////////////////////////////////////////////////////////
//
//	hex_to_string
//
///////////////////////////////////////////////////////////////////////////////
//
template <class from_t> std::string hex_to_string(const from_t& t) 
{ 
	std::ostringstream os; 
	os << std::hex << t;
	return os.str();
}



///////////////////////////////////////////////////////////////////////////////
//
//	from_string
//
///////////////////////////////////////////////////////////////////////////////
//
template<class to_t> to_t from_string(const std::string& s)
{
	to_t r = 0;
	std::istringstream(s) >> (s.find("x")==std::string::npos ? std::dec : std::hex) >> r;
	return r;
}
//\endcond



namespace pal
{
uint32	time()				throw(tcat::exception::base);			///< return current time in milli seconds
uint64	time64()			throw(tcat::exception::base);			///< return current time in micro seconds
uint64	time64wrap()		throw(tcat::exception::base);			///< return current time in micro seconds expand to 32 to 64
void	delay	(uint32 ms)	throw(tcat::exception::base);			///< waits the thread for milli seconds
void	swap32	(void* data32, uint32 words32=1);					///< inplace endian swaps 
void	swap32	(void* dst32, const void* src32, uint32 words32=1);	///< swaps while copying data
}// namespace pal



namespace dice
{
///////////////////////////////////////////////////////////////////////////////
//
//	exceptions
//
///////////////////////////////////////////////////////////////////////////////
//
TCAT_EXCEPTION_DECLARE (xptn_internal,					tcat::exception::base,	"Internal I/O platform abstraction layer error");	///<	Internal I/O platform abstraction layer error"
TCAT_EXCEPTION_DECLARE (xptn_internal_interface,		xptn_internal,			"Internal interface mismatch");						///<	Internal interface mismatch
TCAT_EXCEPTION_DECLARE (xptn_resource,					tcat::exception::base,	"System resource exhausted");						///<	System resource exhausted
TCAT_EXCEPTION_DECLARE (xptn_resource_timer,			tcat::exception::base,	"System did not implement high resolution timer");	///<	System did not implement high resolution timer
TCAT_EXCEPTION_DECLARE (xptn_resource_file,				xptn_resource,			"File open error");									///<	File open error
TCAT_EXCEPTION_DECLARE (xptn_resource_firmware,			xptn_resource,			"Invalid firmware binary");							///<	Invalid firmware binary
TCAT_EXCEPTION_DECLARE (xptn_driver,					xptn_internal,			"Driver not installed");							///<	Driver not installed
TCAT_EXCEPTION_DECLARE (xptn_driver_version,			xptn_driver,			"Driver version mismatch");							///<	Driver version mismatch
TCAT_EXCEPTION_DECLARE (xptn_driver_access,				xptn_driver,			"Driver access error");								///<	Driver access error
TCAT_EXCEPTION_DECLARE (xptn_driver_locked,				xptn_driver,			"Driver streaming parameter locked");				///<	Driver streaming parameter locked
TCAT_EXCEPTION_DECLARE (xptn_device,					xptn_driver_access,		"Device is not accessible");						///<	Device is not accessible
TCAT_EXCEPTION_DECLARE (xptn_device_access,				xptn_device,			"Device access failed");							///<	Device access failed
TCAT_EXCEPTION_DECLARE (xptn_device_timeout,			xptn_device,			"Device operation timeout");						///<	Device operation timeout
TCAT_EXCEPTION_DECLARE (xptn_device_firmware,			xptn_device,			"Device firmware operation error");					///<	Device firmware operation error
TCAT_EXCEPTION_DECLARE (xptn_device_firmware_timeout,	xptn_device_firmware,	"Device firmware operation timeout");				///<	Device firmware operation timeout
//TCAT_EXCEPTION_DECLARE (xptn_device_firmware_version, xptn_device_firmware,	"Device firmware version mismatch");				///<	Device firmware version mismatch


	
class lock;
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base
//
///////////////////////////////////////////////////////////////////////////////
///
///	The base object handles the following purposes:
///	- implements locking mechanism
///	- implements recursive reference tracking
///
class base
{
	friend class lock;
	typedef std::deque<const base*> base_list;
public:
			void		_addref(const base*owner=NULL);					///< Adds a referencing object. (Direct use is deprecated - use reference class)
			bool		_release(const base*owner);						///< Release the reference. (Direct use is deprecated - use reference class)
protected:
						base():	
							m_are_all_objects_known_recursion(false),
							m_is_object_known_recursion(false),
							m_release_recursion(false)	{}				///< Protected base class constructor (base is an abstract class and needs to be overwritten)
//	virtual				~base();										///< Protected base class destructor (base is an abstract class and needs to be overwritten)
#ifdef WIN32
			bool		test();											///< test interface for the lock class
#endif //WIN32
private:
			bool		_are_all_objects_known() const;
			bool		_is_object_known(const base*object) const;
			void		_lock()			throw(tcat::exception::base);	///< Private interface for the lock class
			void		_unlock()		throw(tcat::exception::base);	///< Private interface for the lock class
//\cond
			base_list	m_owner_list;									///< List of base objects that are holding a reference to this object 
			bool		m_are_all_objects_known_recursion;
			bool		m_is_object_known_recursion;
			bool		m_release_recursion;							///< Re-releasing prevention when releasing recursive references
//\endcond
};



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::lock
//
///////////////////////////////////////////////////////////////////////////////
///
///	\brief Auto lock stack class
///
///	The auto lock helper class is to be used as a stack object together with a
/// base class derivate. It locks the object and releases it when it gets out
///	of focus.
///
/// Locking a base class derivate prevents from any state change of this object.
///
class lock
{
public: 
			lock(base*object): m_object(object)	{ _lock(); }								///< Lock object constructor: locks the paremeter object
			~lock()								{ _unlock(); }								///< Lock object destructor: un-locks the paremeter object
	void	operator=(base* object)				{ _unlock(); m_object=object; }				///< Reference object assignment
//	void	operator=(const self& reference)	{ _unlock(); m_object=reference.m_object; }	///< Reference object assignment
//	void	object(base*object)					{ m_object=object; }	///< To assign 
//\cond
private:
	void	_lock()								{ if (m_object) m_object->_lock(); }
	void	_unlock()							{ if (m_object) m_object->_unlock(); }
	base*	m_object;
//\endcond
};



///////////////////////////////////////////////////////////////////////////////
/// 
///	\class tcat::dice::reference<>
///
///////////////////////////////////////////////////////////////////////////////
///
///	\brief Auto reference helper class
///
///	This is the auto reference helper class. The dice::reference<> wraps 
///	around an object pointer and is to be used as it was a pointer (except it 
///	does not need to be released).\n
///	Referenced objects must implement an addref() and release() and keep track 
///	of the objects referencing it.\n
///	Using references can reduce source code, increase readability and stability 
///	by keeping track of correct dereferencing - even in case of exceptions. If 
///	the reference object gets out of focus it releases its reference.\n
///	All object pointer operations, that were valid with the pointer object will 
///	also work with this reference and are source code compatible.\n
///	Object pointers are automatically converted into references.\n
///	Please notice:\n
///	References are used to referencing to objects of the dynamically created 
///	device tree. Holding a reference is different from holding a lock. A 
///	reference does not prevent from any kind of notification/change to the tree
/// structure etc. It only guarantees that the object referencing to is still 
///	valid (until the referene object goes out of focus).
///
template<class object_t=base> class reference
{
	typedef reference<object_t> self;
public: 
							reference(): m_object(NULL), m_owner(NULL) {}										///< reference default constructor
							reference	(const self& reference)	{ m_object=reference.m_object; m_owner=reference.m_owner; if (m_object) m_object->_addref(m_owner); }	
																												///< reference copy constructor
							reference	(object_t*object,base*owner=NULL): m_object(object), m_owner(owner)
																{ if (m_object) m_object->_addref(m_owner); }	///< reference initialising constructor: takes the object to reference as a parameter
							~reference()						{ _release(); }									///< reference destructor
	void					owner		(base*owner)			{ m_owner=owner; }								///< Sets the owner for member references of base class derivates
	object_t*	operator	->()								{ return m_object; }							///< Object pointer
	object_t*	operator	->() const							{ return m_object; }							///< Const object pointer
				operator	object_t*()							{ return m_object; }							///< Auto converter into object pointer
				operator	object_t*() const					{ return m_object; }							///< Auto converter into object pointer
	void		operator	=(object_t* object)					{ _release(); m_object=object; if (m_object) m_object->_addref(m_owner); }				///< Reference object assignment
	void		operator	=(const self& reference)			{ _release(); m_object=reference.m_object; if (m_object) m_object->_addref(m_owner); }	///< Reference object assignment

	template <class element_t>	element_t*	at	(size_t idx)	{ lock lock(m_object); if (m_object->size()>idx) return dynamic_cast<element_t*>((dice::base*)m_object->at(idx)); else return NULL; }	///< Deprecated: lock and dynamic cast to list objects
//\cond
protected:
	void					_release()							{  if (m_object && m_object->_release(m_owner)) { delete m_object; m_object=NULL;} }
private:
	object_t*	m_object;
	base*		m_owner;
//\endcond
};




#ifdef __MACH__
//\cond
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::mutex
//
///////////////////////////////////////////////////////////////////////////////
//
class mutex
{
public:
	mutex(pthread_mutex_t* pthread_mutex)	throw(xptn_resource): m_pthread_mutex(pthread_mutex)	{ TCAT_EXCEPTION_THROW (::pthread_mutex_lock(m_pthread_mutex), xptn_resource); }
	~mutex()								throw(xptn_resource)									{ TCAT_EXCEPTION_THROW (::pthread_mutex_unlock(m_pthread_mutex), xptn_resource); }
private:
	pthread_mutex_t* m_pthread_mutex;
};
//\endcond
#endif //__MACH__

} // namespace dice
} // namespace tcat
