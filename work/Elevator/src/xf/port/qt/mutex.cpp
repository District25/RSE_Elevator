#include <cassert>
#include <config/xf-config.h>

#if (USE_XF_PORT_QT_MUTEX_CLASS != 0)

#include "mutex.h"

/**
 * @brief Implementation of interface::XFMutex::create method.
 */
interface::XFMutex * interface::XFMutex::create()
{
    return new ::XFMutex;
}

void XFMutex::lock()
{
    mutex_.lock();
}

void XFMutex::unlock()
{
    mutex_.unlock();
}

bool XFMutex::tryLock(int32_t timeout /* = 0 */)
{
    return mutex_.tryLock(timeout);
}

#endif // USE_XF_PORT_QT_MUTEX_CLASS
