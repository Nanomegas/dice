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
//	TCAT exception handling
//

#pragma once

#include <exception>
#include <string>



#ifdef WIN32 
	// IMPORTANT: .Net 2003 does not support exception specification. It accepts the
	// syntax but no additional code is generateg. Additionally warning 4290 is 
	// posted. Since this can be very anoying, we disable it !
	#pragma warning(disable: 4290)
#endif



namespace tcat
{
namespace exception
{



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::exception::base
//
///////////////////////////////////////////////////////////////////////////////
///
///	tcat exception framework
///
class base: virtual public std::exception
{
public:
							base(): m_filename(""), m_name(""), m_dbg_msg(""), m_usr_msg(""),  m_line(0) {}
//							base(const base&base): m_filename(""), m_name(""), m_dbg_msg(""), m_usr_msg(""),  m_line(0) {}
							base(std::string filename, int line, std::string dbg_msg, std::string usr_msg, std::string name="base");	///< Exception base class constructor
    virtual					~base() throw() {}																							///< Exception base class destructor

	static 	void			enable	(const std::string exceptions[]);	///< Enables exception handling for a named exception
	
			std::string 	filename()	const	{ return m_filename; }	///< Return the source code filename where the exception occured
			std::string		dbg_msg()	const	{ return m_dbg_msg; }	///< Return an additional debug message
			std::string		usr_msg()	const	{ return m_usr_msg; }	///< Return the user displayable message
			std::string 	name()		const	{ return m_name; }		///< Return the exception name
			int			 	line()		const	{ return m_line; }		///< Return the line number of the source code where the exception occured
//\cond
protected:			 					 					
			std::string		m_filename;
			std::string		m_name;
			std::string		m_dbg_msg;
			std::string		m_usr_msg;
			int	   			m_line;
//\endcond
};



///////////////////////////////////////////////////////////////////////////////
//
//	TCAT_EXCEPTION_DECLARE
//
///////////////////////////////////////////////////////////////////////////////
//
//	macro to declare and define exceptions.
//
#define TCAT_EXCEPTION_DECLARE(name,base,usr_msg_default)																				\
	class name: public base																												\
	{																																	\
	public:																																\
			name (std::string filename, int line, std::string dbg_msg, std::string usr_msg=usr_msg_default, std::string name=#name"->"#base):	\
				base(filename,line,dbg_msg,usr_msg,name){}																				\
	virtual	~name() throw(){}																											\
	} 		 																															\
								 


///////////////////////////////////////////////////////////////////////////////
//
//	TCAT_EXCEPTION_THROW
//
///////////////////////////////////////////////////////////////////////////////
//
//	macro to conditionally throw an exception
//
#define TCAT_EXCEPTION_THROW(condition,exception)	\
	((condition) ? throw exception(__FILE__, __LINE__, std::string(TCAT_FUNCTION) + ": \"" #condition "\""):(void)0)



///////////////////////////////////////////////////////////////////////////////
//
//	TCAT_EXCEPTION_TRY
//
///////////////////////////////////////////////////////////////////////////////
//
//	macro to rethrow an exception from within a more describing context	
//
#define TCAT_EXCEPTION_TRY(cmd,base,exception)	\
	try { cmd; } catch(base tmp) { throw exception(__FILE__, __LINE__, std::string(TCAT_FUNCTION) + ": \"" #cmd "\" from: [" + tmp.name() + ": " + tmp.usr_msg() + "; " + tmp.dbg_msg() + ";" + tmp.filename() + /* "; line " + tmp.line() +*/ "]"); }



}// namespace exception
}// namespace tcat
