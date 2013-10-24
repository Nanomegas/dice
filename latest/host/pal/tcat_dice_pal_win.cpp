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
//	see tcat_dice_pal.h
//

#include "tcat_dice_pal.h"
#include "windows.h"

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
	return ::timeGetTime();
//	return ::GetTickCount();
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::time
//
///////////////////////////////////////////////////////////////////////////////
//
uint64 time64() throw(tcat::exception::base)
{
	try
	{
		LARGE_INTEGER frequency;
		LARGE_INTEGER counter;
		TCAT_EXCEPTION_THROW (!::QueryPerformanceFrequency (&frequency), xptn_resource);
		TCAT_EXCEPTION_THROW (!::QueryPerformanceCounter (&counter), xptn_resource);
		return (counter.QuadPart*1000000)/frequency.QuadPart;
	}
	catch (xptn_resource)
	{
		return ::timeGetTime()*1000;
	}
}

//ML:ugly hack, we don't like global vars.

static uint64 last64bitTime=0;

uint64 time64wrap() throw(tcat::exception::base)
{
	try
	{
		LARGE_INTEGER frequency;
		LARGE_INTEGER counter;
		
		TCAT_EXCEPTION_THROW (!::QueryPerformanceFrequency (&frequency), xptn_resource);
		TCAT_EXCEPTION_THROW (!::QueryPerformanceCounter (&counter), xptn_resource);
		//compensate for the fact that on some chipsets this counter is only 32 bits.
		if (((uint32)(counter.QuadPart & 0xffffffff)) < ((uint32)(last64bitTime & 0xffffffff)))
		{
			//we had a wrap
			last64bitTime += ((uint64)1<<32);
		}
		last64bitTime = (last64bitTime & 0xffffffff00000000) | (counter.QuadPart & 0xffffffff);

		return (uint64)(((double)last64bitTime*1000000.0)/(double)frequency.QuadPart);
		
	}
	catch (xptn_resource)
	{
		return ::timeGetTime()*1000;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::delay
//
///////////////////////////////////////////////////////////////////////////////
//
void delay (uint32 ms) throw(tcat::exception::base)
{
	::Sleep(ms);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::pal::swap32
//
///////////////////////////////////////////////////////////////////////////////
//
void swap32 (void* data32, uint32 words32)
{
	uint32* ndata32 = (uint32*)data32;
	for (;words32;words32--)
		*ndata32++ = ntohl (*ndata32);
}
void swap32 (void* dst, const void* src, uint32 words32)
{
	uint32* ndst = (uint32*)dst;
	uint32* nsrc = (uint32*)src;
	for (;words32;words32--)
		*ndst++ = ntohl (*nsrc++);
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
		try
		{
			memset (&m_lock, 0, sizeof m_lock);
			::InitializeCriticalSection(&m_lock);
		}
		catch(...)
		{
			TCAT_EXCEPTION_THROW (true, xptn_resource);
		}
	}
	~base_lock()
	{
		::DeleteCriticalSection(&m_lock);
	}
	void _lock() throw(tcat::exception::base)
	{
		try
		{
			::EnterCriticalSection(&m_lock);
		}
		catch(...)
		{
			TCAT_EXCEPTION_THROW (true, xptn_resource);
		}
	}
	void _unlock() throw(tcat::exception::base)
	{
		::LeaveCriticalSection(&m_lock);
	}
	bool _test() throw(tcat::exception::base)
	{
		return (::TryEnterCriticalSection(&m_lock) != 0);
	}
protected:
	CRITICAL_SECTION	m_lock;
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



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::test
//
///////////////////////////////////////////////////////////////////////////////
//
bool base::test()
{
	return __base_lock._test();
}
