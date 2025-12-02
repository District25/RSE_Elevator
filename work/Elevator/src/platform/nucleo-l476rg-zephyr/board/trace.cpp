#include <config/trace-config.h>

#if (USE_PLATFORM_F7_DISCO_ZEPHYR_TRACE != 0)

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <zephyr/sys/printk.h>
#include "trace/trace.h"        // Include the general trace.h file (common for many projects)
#include "xf/interface/mutex.h"

#ifndef TRACE_ADD_CRLF_SEQU
#define TRACE_ADD_CRLF_SEQU 	0
#endif

using interface::XFMutex;

static char strTrace[255];

static int32_t checkStringEnding(char * str, uint32_t len);

static interface::XFMutex & traceMutex()
{
	static XFMutex * mutex = XFMutex::create();
	return *mutex;
}

static void traceMutexLock()
{
	traceMutex().lock();
}

static void traceMutexUnlock()
{
	traceMutex().unlock();
}

void trace_initialize()
{
    Trace::initialize();
}

void trace_out(const char * const format , ...)
{
    va_list args;

    // Format string
    va_start(args, format);
    vsprintf(strTrace, format, args);
    va_end(args);

    checkStringEnding(strTrace, strlen(strTrace));

	printk("%s", strTrace);
}

//static
void Trace::initialize()
{

}

static int32_t checkStringEnding(char * str, uint32_t len)
{
	if (!len)
	{
		return 0;
	}

	if (str[len-1] != '\n')
	{
#if (TRACE_ADD_CRLF_SEQU != 0)
		// Add "\r\n" at the end of the string
		str[len] = '\r';
		str[len+1] = '\n';
		str[len+2] = '\0';
#else
		// Add "\n" at the end of the string
		str[len] = '\n';
		str[len+1] = '\0';
#endif // TRACE_ADD_CRLF_SEQU
		}
#if (TRACE_ADD_CRLF_SEQU != 0)
	// Check string finishing with "\r\n"
	else if (str[len-1] == '\n')
	{
		// Check if second last char is an '\r'
		if (len == 1 ||
			(len >= 2 && str[len-2] != '\r'))
		{
			// Squeeze a '\r'
			memmove(&str[len], &str[len-1], 2);
			str[len-1] = '\r';
		}
	}
#endif // TRACE_ADD_CRLF_SEQU
	return strlen(str);
}

void Trace::out(string str)
{
	traceMutexLock();
	if (str[str.length()-1] != '\n')
	{
		str += '\n';
	}

#if defined(TRACE_ADD_CRLF_SEQU) && (TRACE_ADD_CRLF_SEQU != 0)
	if(str[str.length()-2] != '\r'){
		str.insert(str.length()-1, "\r");
	}
#endif // TRACE_ADD_CRLF_SEQU

	printk("%s", str.data());
	traceMutexUnlock();
}

void Trace::out(const char * format, ...)
{
	traceMutexLock();
	va_list args;

	// Format string
	va_start(args, format);
	vsprintf(strTrace, format, args);
	va_end(args);

	checkStringEnding(strTrace, strlen(strTrace));

	printk("%s", strTrace);
	traceMutexUnlock();
}

//static
void Trace::lock()
{
    traceMutexLock();
}

//static
void Trace::unlock()
{
    traceMutexUnlock();
}

#endif // USE_PLATFORM_F7_DISCO_ZEPHYR_TRACE
