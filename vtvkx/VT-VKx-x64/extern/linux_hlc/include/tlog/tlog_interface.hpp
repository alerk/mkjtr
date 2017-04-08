#ifndef TLOG_INTERFACE_HPP_
#define TLOG_INTERFACE_HPP_

// class publish_location
// and #defines

#include <tlog/tlog_types.hpp>

namespace TLOG
{
  // register publish location (only once) and publish message
  extern void Register_Location (publish_location*, int, const char* format, ... )
    PRINTF_FP(3,4);
  
  // register publish location (only once) and publish message
  extern void Publish (publish_location*, int, const char* format, ... )
    PRINTF_FP(3,4);
    
  // init logging without using separate thread
  extern void Init (const char* fProfileName, const char* fComponentName);
    
  // init logging by using separate thread
  extern void Init (const char* fProfileName, const char* fComponentName, int fSchedPolicy, int fPriority, int fStacksize, bool fDetached);
  
  extern void* Read_Profile (void* fArgPtr);
  
  extern int Update (void);
}

#endif // TLOG_INTERFACE.HPP_  

