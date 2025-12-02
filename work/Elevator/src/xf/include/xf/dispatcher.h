#ifndef XF_CORE_DISPATCHER_H
#define XF_CORE_DISPATCHER_H

#include <config/xf-config.h>

#if (USE_XF_CORE_DISPATCHER_CLASS != 0)

#include "xf/interface/dispatcher.h"
#include "xf/interface/thread.h"
#include "eventqueue.h"

/** @ingroup xf_core
 *  @{
 */

/**
 * @brief A dispatcher implementation which may be executed using an internal thread.
 *
 * The active dispatcher internally creates a Thread and starts it with the start() method.
 * The thread itself is responsible to execute the protected method execute().
 *
 * In case the dispatcher is passive, another thread dedicated to the XF needs to execute the dispatcher.
 */
class XFDispatcher : public interface::XFThreadEntryPointProvider,
                     public interface::XFDispatcher
{
public:
    /**
     * @brief Default XFDispatcher constructor.
     * @param isActive Set to true to create an internal thread.
     */
    XFDispatcher(bool isActive = true, const char * threadName = nullptr);
    ~XFDispatcher();

    bool isActive() const override { return (pThread_ != nullptr); }    ///< Returns true if the internal thread is executing the dispatcher.

    void start() override;
    void stop();                                                        ///< Stops execution of the dispatcher (and internal thread).
    void pushEvent(XFEvent * pEvent, uint32_t timeout = 0) override;
    XFEventHandle pushEvent(int eventId, uint32_t timeout, interface::XFBehavior * pBehavior) override;

    void executeOnce() override;

protected:
    int execute(const void * param = nullptr) override;
    void dispatchEvent(const XFEvent * pEvent) const override;

protected:
    bool isExecuting_;                  ///< True as long as the thread is executing the main loop.
    interface::XFThread * pThread_;     ///< Pointer to Thread executing the dispatcher.
    XFEventQueue events_;               ///< Thread-safe queue holding events waiting to get dispatched.
};

/** @} */ // end of xf_core group
#endif // USE_XF_CORE_DISPATCHER_CLASS
#endif // XF_CORE_DISPATCHER_H
