#ifndef TLOG_TYPES_HPP_
#define TLOG_TYPES_HPP_

#include <fstream>
#include <string>
#include <vector>


namespace TLOG
{
  #define TLOG_MIN_FILE_SIZE 4096
  #define TLOG_PROFILE_DEFAULT_CHECK_TIME  300
  #define TLOG_PROFILE_MIN_CHECK_TIME 10
  #define TLOG_FILE_MESSAGE_BLOCK_LENGTH  128
  #define TLOG_MAX_LINE_STRING_LENGTH  10
  #define TLOG_MAX_LOGGING_MESSAGE_LENGTH  255
 
  // the __attribute__ mechanism allows a developer to attach characteristics to function
  // decalration allowing the compiler to perform more error checking
  // In this case parameter types against printf formats are checked
  // FM is position of format string and X ist position of first parameter
  #ifdef WIN32
  #define PRINTF_FP(FMT,X) __attribute__(( __format__ ( gnu_printf, FMT, X)))
  #else
  #define PRINTF_FP(FMT,X) __attribute__(( __format__ ( __printf__, FMT, X)))
  #endif
  
  // struct publish_location
  // static struct for each module

  // Structure created for each module to keep track of logging state
  // and associated data.
    
  class file_description
  {
  public:
    // constructors
    inline file_description (void);    
    inline file_description (const std::string& fName, int fSize, std::fstream* fFilePtr);
    
    // descructor
    inline ~file_description (){};
    
    // returns max size of file
    inline int size (void) const;
    
    // sets file size
    inline void set_size (int fSize);
    
    // retrun file name
    inline std::string name (void) const;
    
    // returns file handle
    inline std::fstream* handle (void) const;
    
    // close file
    inline void close (void);
    
  private:    
  
    // max file size
    int mSize;
    
    // file pointer
    std::fstream* mFilePtr;
    
    // file name
    std::string mName;

    // hide copy constructor and operator=
    file_description(const file_description&);
    file_description& operator=(const file_description&);

  };
  
  class publish_manager;
  class location;
 
	typedef enum
	{
	  TLOG_UNDEFINED_LEVEL=0, // undefined level
	  TLOG_ASSERTION_LEVEL,   // assertion failed	
	  TLOG_ERROR_LEVEL,       // error conditions	
	  TLOG_WARNING_LEVEL,     // warning conditions	
	  TLOG_INFO_LEVEL,        // informational	        
	  TLOG_DEBUG_LEVEL        // debug level message
  } logging_level;
    	
  struct publish_location
  {
	  // the compiler supports printf attribute specification on function
	  // pointers, we'll use it here so that the compiler knows to check for
	  // proper printf formatting.  	
    void (*publish) (publish_location*, int, const char* format, ... )
			PRINTF_FP(3,4);		
	  logging_level    mLevel;
	  const char*      mModuleName;
	  const char*      mFunctionName;
	  int              mLine;    							
	  publish_manager* mPublishManagerPtr;
	  location*        mParentPtr;
  };
  
  typedef std::vector<std::string> string_list;   
  
  typedef struct
  {
    std::string  mTime;
    const char*  mFunctionName;
    const char*  mModuleName;
    int          mLine;
    char*        mMsg;
    logging_level mLevel;
    int           mId;
  } trace_data;      
}

inline TLOG::file_description::file_description (void)
  : mSize (0),
    mFilePtr (0),
    mName ("")
{
}

inline TLOG::file_description::file_description (const std::string& fName, int fSize, std::fstream* fFilePtr)
  : mSize (fSize),
    mFilePtr (fFilePtr),
    mName (fName)
{
}      

inline void
TLOG::file_description::close(void)
{
  if (mFilePtr)
  {
    mFilePtr->close();
    delete mFilePtr;
    mFilePtr = NULL;
  }
}

inline int
TLOG::file_description::size (void) const
{      
  return mSize;
}

inline void
TLOG::file_description::set_size (int fSize)
{      
  mSize = fSize;
}  

inline std::string
TLOG::file_description::name (void) const
{      
  return mName;
}

inline std::fstream*
TLOG::file_description::handle (void) const
{      
  return mFilePtr;
}


#endif // TLOG_TYPES_HPP_



