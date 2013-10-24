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

#include "tcat_dice_pal.h"


using namespace tcat;
using namespace dice;



///////////////////////////////////////////////////////////////////////////////
//
//	debugging
//
///////////////////////////////////////////////////////////////////////////////
//
#ifdef TCAT_DEBUG
	#ifdef __MACH__
		#ifdef __cplusplus
		extern "C" {
		#endif
			extern void Debugger(void);
		#ifdef __cplusplus
		}
		#endif
	#endif
#endif

void pal_Debugger()
{
#ifdef TCAT_DEBUG
	#ifdef __MACH__
		::Debugger();
	#endif
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::addref
//
///////////////////////////////////////////////////////////////////////////////
//
void base::_addref(const base*owner)
{
	lock lock(this);
	m_owner_list.push_back(owner);
}



//\cond
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::release
//
///////////////////////////////////////////////////////////////////////////////
///
/// This object is to be deleted if all referencees to this objects are only 
///	reverse references of this object or objects that are directly or 
///	indirectly referenced by this object - AND - if the objects referencing it 
///	have the same conditions.
///
///	This is the case if the number of this objects references equals the number 
///	of references to this objects - that can be reached thru the reference 
///	track.
///
bool base::_release(const base*owner)
{
	bool release = false;
	{
		lock lock(this);	
		
		// has the iteration been here before?
		if (!m_release_recursion)
		{
			m_release_recursion=true; // this prevents from re-releasing when releasing ring references
			m_owner_list.erase(std::find(m_owner_list.begin(), m_owner_list.end(), owner));
			if (_are_all_objects_known())
				release=true;
			else
				m_release_recursion=false;
		}
	}
	return release;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::_are_all_objects_known
//
///////////////////////////////////////////////////////////////////////////////
///
///	returns true if all objects of the owner tree are referenced 
///
bool base::_are_all_objects_known() const 
{
	bool are_all_objects_known = true;
	if (!m_are_all_objects_known_recursion)
	{
		const_cast<base*>(this)->m_are_all_objects_known_recursion=true;
		for (base_list::const_iterator owner=m_owner_list.begin(); owner!=m_owner_list.end() && are_all_objects_known; owner++)
		{
			if (*owner && _is_object_known(*owner))
				are_all_objects_known = (*owner)->_are_all_objects_known();
			else
				are_all_objects_known = false;
		}
		const_cast<base*>(this)->m_are_all_objects_known_recursion=false;
	}
	return are_all_objects_known;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::base::_is_object_known
//
///////////////////////////////////////////////////////////////////////////////
///
///	returns true if this object of the owner tree is referenced 
///
bool base::_is_object_known (const base*object) const
{
	bool is_object_known = false;
	if (!m_is_object_known_recursion)
	{
		const_cast<base*>(this)->m_is_object_known_recursion=true;
		for (base_list::const_iterator owner=m_owner_list.begin(); owner!=m_owner_list.end() && !is_object_known; owner++)
		{
			if (*owner==object)
				is_object_known = true;
			else if (*owner)
				is_object_known = (*owner)->_is_object_known(object);
		}
		const_cast<base*>(this)->m_is_object_known_recursion=false;
	}
	return is_object_known;
}
//\endcond
