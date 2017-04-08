#ifndef TLOG_PRE_PROCESSOR_HPP_
#define TLOG_PRE_PROCESSOR_HPP_

#ifdef _TLOG_LOGGING_

#define STR(X) #X
#define CONCAT2(A,B) A##B
#define CONCAT(A,B) CONCAT2(A,B)
#define TLOGID CONCAT(_Log_, __LINE__)

// the __attribute__ mechanism allows a developer to attach characteristics to function
// decalration allowing the compiler to perform more error checking
// In this case parameter types against printf formats are checked
// FM is position of format string and X ist position of first parameter
//#define PRINTF_FP(FMT,X) __attribute__(( __format__ ( __printf__, FMT, X)))		

#include <tlog/tlog_types.hpp>
#include <tlog/tlog_interface.hpp>
#include <unistd.h>


#define TLOG_INIT_ONCE(PROFILE, COMPONENT)\
	TLOG::Init (PROFILE, COMPONENT)
	
#define TLOG_INIT_CYCLIC(PROFILE, COMPONENT, SCHEDPOLICY, PRIORITY, STACKSIZE, DETACHED)\
	TLOG::Init (PROFILE, COMPONENT, SCHEDPOLICY, PRIORITY, STACKSIZE, DETACHED)	

//#define TLOG_INIT_CYCLIC (PROFILE, COMPONENT,

// def _logMessageDef
//  defines a static PublishLoc and points it to the registration function for
//  the first call
// note, that initialization of of non global static variables is thread safe (if it is not cached as return value)
#define _logMessageDef(ID, LEVEL) \
  static TLOG::publish_location ID ={&TLOG::Register_Location, LEVEL, \
      __FILE__, __PRETTY_FUNCTION__, __LINE__, NULL, NULL};

// def _logMessageCall
// checks if the RLogPublisher is enabled and publishes the message if so.
// internal
#define _logMessageCall(ID, format, ...) \
  if(ID.publish!=0) (*ID.publish)( &ID, (int)getpid(), format, ##__VA_ARGS__ );


#define _logMessage(ID, LEVEL, format, ... ) \
  do { _logMessageDef(ID, LEVEL) \
       _logMessageCall(ID, format, ##__VA_ARGS__ ) } while(0)

// these macros are the primary interface for logging messages:
// - TLOG_DEBUG(format, ...)
// - TLOG_INFO(format, ...)
// - TLOG_WARNING(format, ...)
// - TLOG_ERROR(format, ...)
// - TLOG_ASSERT( condition )

// macros for user interface
// debug level messages
#define TLOG_DEBUG(format, ...) \
  _logMessage( TLOGID, TLOG::TLOG_DEBUG_LEVEL, format, ##__VA_ARGS__ )    

// info level messages
#define TLOG_INFO(format, ...)  \
	_logMessage (TLOGID, TLOG::TLOG_INFO_LEVEL, format, ##__VA_ARGS__ )	
	
// warning level messages
#define TLOG_WARNING(format, ...) \
	_logMessage (TLOGID, TLOG::TLOG_WARNING_LEVEL, format, ##__VA_ARGS__ )
	
// error level messages
#define TLOG_ERROR(format, ...) \
	_logMessage (TLOGID, TLOG::TLOG_ERROR_LEVEL, format, ##__VA_ARGS__ )	
		
// Assert error condition, and display message/condition if assertion fails
// use do-while construct in order to use it in condition statements
#define TLOG_ASSERT(cond) \
    do { \
	if(cond == false) \
	{ _logMessage(TLOGID, TLOG::TLOG_ASSERTION_LEVEL, STR(cond)); }\
	} while(0)
    
#define TLOG_ENTER_FUNCTION\
  TLOG_DEBUG ("Enter function \"%s\" ...", __FUNCTION__);
  
#define TLOG_LEAVE_FUNCTION\
  TLOG_DEBUG ("Leave function \"%s\" ...", __FUNCTION__);

#else  // _LOGGING_

// macros for user interface
// debug level messages
#define TLOG_DEBUG(format, ...)

// info level messages
#define TLOG_INFO(format, ...)
	
// warning level messages
#define TLOG_WARNING(format, ...)
	
// error level messages
#define TLOG_ERROR(format, ...)
		
// Assert error condition, and display message/condition if assertion fails
// use do-while construct in order to use it in condition statements
#define TLOG_ASSERT(cond)
    
#define TLOG_ENTER_FUNCTION
  
#define TLOG_LEAVE_FUNCTION

#define TLOG_INIT_ONCE(PROFILE, COMPONENT)

#define TLOG_INIT_CYCLIC(PROFILE, COMPONENT, SCHEDPOLICY, PRIORITY, STACKSIZE, DETACHED)

#endif // _LOGGING_


#endif // TLOG_PRE_PROCESSOR_HPP_

