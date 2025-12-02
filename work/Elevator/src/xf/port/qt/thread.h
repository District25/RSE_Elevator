#ifndef XF_THREAD_QT_H
#define XF_THREAD_QT_H

#include <config/xf-config.h>

#if (USE_XF_PORT_QT_THREAD_CLASS != 0)

#include <QThread>
#include <cstdint>
#include "xf/interface/thread.h"

/** @ingroup port_qt
 *  @{
 */

/**
 * @brief Default thread implementation for the IDF Qt port
 */
class XFThread : public QThread,
                 public interface::XFThread
{
    friend class interface::XFThread;

public:
    void start() override;
    void stop() override;

    void setPriority(XFThreadPriority priority) override;
    XFThreadPriority getPriority() const override;

    void delay(uint32_t milliseconds) override;

protected:
    /**
     * @brief Protected default constructor
     * @param pProvider Instance providing the method to be executed by the thread.
     * @param entryMethod Method to be executed by the Thread (usually containing a infinite loop).
     * @param threadName Name of the thread.
     * @param stackSize Stack size of the thread.
     *
     * Constructor is protected because only the XFThread (and the XFThread interface)
     * is allowed to created threads.
     */
    XFThread(interface::XFThreadEntryPointProvider * pProvider,
                    interface::XFThread::EntryMethodBody entryMethod,
                    const char * threadName,
                    const uint32_t stackSize = 0);

    // QThread interface implementation
protected:
    void run() override;                                                ///< Override of the QThread run method.

protected:
    interface::XFThreadEntryPointProvider * pEntryMethodProvider_;      ///< Pointer to object providing the entry method.
    interface::XFThread::EntryMethodBody entryMethod_;                  ///< Entry method to be called/executed by the thread.
};

/** @} */ // end of port_qt group
#endif // USE_XF_PORT_QT_THREAD_CLASS
#endif // XF_THREAD_QT_H
