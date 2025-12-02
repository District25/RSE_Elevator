#ifndef XF_PORT_ZEPHYR_THREAD_H
#define XF_PORT_ZEPHYR_THREAD_H

#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_THREAD_CLASS != 0)

#include <cstdint>
#include <zephyr/kernel.h>
#include "xf/interface/thread.h"

class XFThread : public interface::XFThread
{
    friend class interface::XFThread;
    
public:
    ~XFThread();

    virtual void start() override;
    virtual void stop() override;

    virtual void setPriority(XFThreadPriority priority) override;
    virtual XFThreadPriority getPriority() const override;

    virtual void delay(uint32_t milliseconds) override;

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

    static void threadEntryPoint(void *param);

    static void k_thread_entry_point(void * p1, void * p2, void * p3);

protected:
    interface::XFThreadEntryPointProvider * pEntryMethodProvider_;
    interface::XFThread::EntryMethodBody entryMethod_;

    char threadName_[32];               ///< Holds name given to the thread.
    k_thread_stack_t * stack;           ///< Pointer to created stack (see k_thread_stack_alloc() function).
    static int initialPriority;         ///< Used to assign unique thread priority at creation time.
    struct k_thread threadHandle;       ///< Thread structure/object used by Zephyr.
    k_tid_t threadId;                   ///< Thread identifier. Id received after thread creation.
};

#endif // USE_XF_THREAD_DEFAULT_CMSIS_OS_IMPLEMENTATION
#endif // USE_XF_PORT_ZEPHYR_THREAD_CLASS
