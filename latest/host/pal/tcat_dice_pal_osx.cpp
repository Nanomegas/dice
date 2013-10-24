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
//	see dice_base.h
//
#include <CoreServices/CoreServices.h>

#include "tcat_dice_pal.h"

using namespace tcat;
using namespace dice;


namespace tcat
{
namespace pal
{
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::time
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 time() throw(tcat::exception::base)
{
	uint64 time;
	::Microseconds ((UnsignedWide*) &time);
	return (uint32)(time / 1000);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::time64
//
///////////////////////////////////////////////////////////////////////////////
//
uint64 time64() throw(tcat::exception::base)
{
	uint64 time;
	::Microseconds ((UnsignedWide*) &time);
	return time;
}


uint64 time64wrap() throw(tcat::exception::base)
{
	return time64();
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::delay
//
///////////////////////////////////////////////////////////////////////////////
//
void delay(uint32 ms) throw(tcat::exception::base)
{
//	TCAT_EXCEPTION_THROW(
	::usleep (ms*1000);
//	, xptn_resource);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::swap32
//
///////////////////////////////////////////////////////////////////////////////
//
void swap32( void* data32, uint32 words32 )
{
	int32		*dataPtr = (int32*)data32;

	while (words32--)
	{
		*dataPtr = OSSwapInt32( *dataPtr );
		++dataPtr;
	}
}

void swap32( void* dst32, const void* src32, uint32 words32 )
{
	const int32	*srcPtr = (int32*)src32;
	int32		*dstPtr = (int32*)dst32;

	while (words32--)
	{
		*dstPtr++ = OSSwapInt32( *srcPtr++ );
	}
}


} // namespace pal
} // namespace tcat



//\cond
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base_lock
//
///////////////////////////////////////////////////////////////////////////////
//
class base_lock
{
public:
	base_lock() throw(tcat::exception::base)
	{
		pthread_mutexattr_t lock_attr;
		
		TCAT_EXCEPTION_THROW (::pthread_mutexattr_init (&lock_attr), xptn_resource);
		TCAT_EXCEPTION_THROW (::pthread_mutexattr_settype (&lock_attr,PTHREAD_MUTEX_RECURSIVE), xptn_resource);

		TCAT_EXCEPTION_THROW (::pthread_mutex_init (&m_lock,&lock_attr), xptn_resource);
	}
	~base_lock()
	{
		::pthread_mutex_destroy(&m_lock);
	}
	void _lock() throw(tcat::exception::base)
	{
		TCAT_EXCEPTION_THROW (::pthread_mutex_lock(&m_lock), xptn_resource);
	}
	void _unlock() throw(tcat::exception::base)
	{
		TCAT_EXCEPTION_THROW (::pthread_mutex_unlock(&m_lock), xptn_resource);
	}
protected:
	pthread_mutex_t m_lock;
};

base_lock __base_lock;


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base
//
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::pal::lock
//
///////////////////////////////////////////////////////////////////////////////
//
void base::_lock() throw(tcat::exception::base)
{
	__base_lock._lock();
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::unlock
//
///////////////////////////////////////////////////////////////////////////////
//
void base::_unlock() throw(tcat::exception::base)
{
	__base_lock._unlock();
}
//\endcond



