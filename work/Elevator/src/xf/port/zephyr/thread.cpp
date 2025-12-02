#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_THREAD_CLASS != 0)

#include <cassert>
#include <string.h>
#include "thread.h"

#if !defined(XFTHREAD_ZEPHYR_STACK_SIZE)
    #define XFTHREAD_ZEPHYR_STACK_SIZE 2048
#endif

/**
 * @brief Implementation of interface::XFThread::create method.
 */
interface::XFThread * interface::XFThread::create(interface::XFThreadEntryPointProvider * pProvider,
                                                  interface::XFThread::EntryMethodBody entryMethod,
                                                  const char * threadName,
                                                  const uint32_t stackSize /* = 0 */)
{
    return new ::XFThread(pProvider, entryMethod, threadName, stackSize);
}

int XFThread::initialPriority = 1;

XFThread::XFThread(interface::XFThreadEntryPointProvider * pProvider,
                   interface::XFThread::EntryMethodBody entryMethod,
                   const char * threadName,
                   const uint32_t stackSize /* = 0 */):
                   threadName_ {'X', 'F', 'T', 'h', 'r', 'e', 'a', 'd'}
{
    uint32_t threadStackSize = stackSize;

    pEntryMethodProvider_ = pProvider;
    entryMethod_ = entryMethod;

    if (threadStackSize == 0) { threadStackSize = XFTHREAD_ZEPHYR_STACK_SIZE; }

    // Needs CONFIG_DYNAMIC_THREAD
    stack = k_thread_stack_alloc(threadStackSize, 0);
    assert(stack);

    // Always keep threads after creation in suspended state.
    // Must be explicitly started with start();
    threadId = k_thread_create(&threadHandle, stack,
                               threadStackSize,
                               k_thread_entry_point,
                               this, NULL, NULL,     // p1, p2, p3
                               K_PRIO_PREEMPT(initialPriority++), 
                               0, 
                               K_FOREVER);

    if (threadName)
    {
        strcpy(threadName_, threadName);        
    }
    k_thread_name_set(threadId, threadName_);
}

XFThread::~XFThread()
{
    k_thread_stack_free(stack);
}

// static
void XFThread::threadEntryPoint(void *param)
{
    XFThread * pThis = (XFThread *)param;

    (pThis->pEntryMethodProvider_->*pThis->entryMethod_)(param);
}

// static 
void XFThread::k_thread_entry_point(void * p1, void * p2, void * p3)
{
    XFThread * pThis = (XFThread *)p1;
    (pThis->pEntryMethodProvider_->*pThis->entryMethod_)(p1);
}

void XFThread::start()
{
    assert(threadId != 0); // Check if thread was created
    k_thread_start(threadId);
}

void XFThread::stop()
{
    k_thread_suspend(threadId);
}

void XFThread::setPriority(XFThreadPriority priority)
{
    int prio = priority;
    k_thread_priority_set(threadId, prio);
}

XFThreadPriority XFThread::getPriority() const
{
    const int prio = k_thread_priority_get(threadId);
    XFThreadPriority priority = XF_THREAD_PRIO_LOW;
    
    if (prio >= XF_THREAD_PRIO_MAX)
    {
        priority = XF_THREAD_PRIO_MAX;
    }
    else if (prio >= XF_THREAD_PRIO_HIGH)
    {
        priority = XF_THREAD_PRIO_HIGH;
    }
    else if (prio >= XF_THREAD_PRIO_NORMAL)
    {
        priority = XF_THREAD_PRIO_NORMAL;
    }

    return priority;
}

void XFThread::delay(uint32_t milliseconds)
{
    k_usleep(milliseconds * 1000);
}

#endif // USE_XF_PORT_ZEPHYR_THREAD_CLASS
