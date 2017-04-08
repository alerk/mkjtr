#ifndef TLOG_HPP_
 #define TLOG_HPP_

#ifndef _SIMULA
 #define _TLOG_LOGGING_

 #include <tlog/tlog_pre_processor.hpp>
#else
 extern void   TLOG_DEBUG                  (const char * fmt, ...);
 extern void   TLOG_INFO                   (const char * fmt, ...);
 extern void   TLOG_WARNING                (const char * fmt, ...);
 extern void   TLOG_ERROR                  (const char * fmt, ...);
#endif

#endif // TLOG_HPP_

