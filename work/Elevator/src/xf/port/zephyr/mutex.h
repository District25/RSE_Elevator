#ifndef XF_PORT_ZEPHYR_MUTEX_H
#define XF_PORT_ZEPHYR_MUTEX_H

#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_MUTEX_CLASS != 0)

#include <stdint.h>
#include <zephyr/kernel.h>
#include "xf/interface/mutex.h"

/** @ingroup port_idf_zephr
 *  @{
 */

/**
 * @brief Default Zephyr implementation for the XFMutex interface.
 */
class XFMutex : public interface::XFMutex
{
    friend class interface::XFMutex;
public:

    void lock() override;
    void unlock() override;

    bool tryLock(int32_t timeout = 0) override;

protected:
    XFMutex();      ///< Do not allow to directly create an object of this class. Call interface::XFMutex::create() instead.

protected:
typedef struct k_mutex Mutex;
    Mutex mutex_;          ///< The real mutex.
};

/** @} */ // end of port_idf_zephr group
#endif // USE_XF_PORT_ZEPHYR_MUTEX_CLASS
#endif // XF_PORT_ZEPHYR_MUTEX_H
