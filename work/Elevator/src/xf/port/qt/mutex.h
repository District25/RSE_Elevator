#ifndef XF_PORT_QT_MUTEX_H
#define XF_PORT_QT_MUTEX_H

#include <config/xf-config.h>

#if (USE_XF_PORT_QT_MUTEX_CLASS != 0)

#include <stdint.h>
#include <QMutex>
#include "xf/interface/mutex.h"

/** @ingroup port_qt
 *  @{
 */

/**
 * @brief Qt implementation for the XFMutex interface.
 */
class XFMutex : public interface::XFMutex
{
    friend class XFResourceFactoryPort;
    friend class interface::XFMutex;
public:

    void lock() override;
    void unlock() override;

    bool tryLock(int32_t timeout = 0) override;

protected:
    XFMutex() = default;     ///< Do not allow to directly create an object of this class. Call XFMutex::create() instead.

protected:
    QMutex mutex_;                  ///< The real mutex.
};

/** @} */ // end of port_qt group
#endif // USE_XF_PORT_QT_MUTEX_CLASS
#endif // XF_PORT_QT_MUTEX_H
