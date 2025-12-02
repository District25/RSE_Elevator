#include <config/xf-config.h>

#if (USE_XF_CORE_DISPATCHER_CLASS != 0)

#include <cassert>
#if defined(XF_TRACE_EVENT_PUSH_POP) && (XF_TRACE_EVENT_PUSH_POP != 0)
#include "trace/trace.h"
#endif // XF_TRACE_EVENT_PUSH_POP
#include "xf/interface/behavior.h"
#include "thread.h"
#include "xf/dispatcher.h"

//static
interface::XFDispatcher * interface::XFDispatcher::getDefaultInstance(bool isActive /* = true */)
{
    static ::XFDispatcher defaultDispatcher(/* isActive = */ isActive, "DefaultDispatcher");
    return &defaultDispatcher;
}

//static
interface::XFDispatcher * interface::XFDispatcher::create(const char * threadName /* = nullptr */)
{
    return new ::XFDispatcher(/* isActive = */ true, threadName);
}

XFDispatcher::XFDispatcher(bool isActive /* = true */, const char * threadName /* = nullptr */) :
    isExecuting_(false),
    pThread_(nullptr)
{
    if (isActive)
    {
        // Create internal thread executing the dispatcher
        pThread_ = interface::XFThread::create(this,
                                               interface::XFThread::EntryMethodBody(&XFDispatcher::execute),
                                               threadName);
        assert(pThread_);
    }
}

XFDispatcher::~XFDispatcher()
{
    isExecuting_ = false;

#ifdef XF_PORT_QT
    events_.releaseThread();
#endif // XF_PORT_QT

    if (pThread_)
    {
        pThread_->stop();

        if (pThread_)
        {
            delete pThread_;
        }
    }
}

void XFDispatcher::start()
{
    isExecuting_ = true;

    if (pThread_)
    {
        assert(pThread_);
        pThread_->start();
    }
}

void XFDispatcher::stop()
{
    isExecuting_ = false;
    if (pThread_)
    {
        pThread_->stop();
    }
}

void XFDispatcher::pushEvent(XFEvent * pEvent, uint32_t timeout /* = 0 */)
{
    assert(pEvent);
    assert(pEvent->getBehavior());

#ifdef XF_TRACE_EVENT_PUSH_POP
    Trace::out("Push event: 0x%x", pEvent);
#endif // XF_TRACE_EVENT_PUSH_POP
    if (timeout == 0)
    {
        events_.push(pEvent);
    }
    else
    {
        pEvent->startTimer(timeout);
    }
}

XFDispatcher::XFEventHandle XFDispatcher::pushEvent(int eventId, uint32_t timeout, interface::XFBehavior * pBehavior)
{
    XFEvent * event = new XFEvent(XFEvent::Event, eventId);

    if (event)
    {
        pBehavior->pushEvent(event, timeout);
    }
    return event;
}

int XFDispatcher::execute(const void * param /* = nullptr */)
{
    (void)param;

    while (isExecuting_)
    {
        while (events_.empty() && isExecuting_)
        {
            events_.pend(); // Wait until something to do
        }

        executeOnce();
    }

    return 0;
}

void XFDispatcher::executeOnce()
{
    if (!events_.empty())
    {
        const XFEvent *pEvent;

        // Deque next event from queue
        pEvent = events_.front();
        events_.pop();
#if defined(XF_TRACE_EVENT_PUSH_POP) && (XF_TRACE_EVENT_PUSH_POP != 0)
        Trace::out("Pop event:  0x%x", pEvent);
#endif // XF_TRACE_EVENT_PUSH_POP

        if (pEvent)
        {
            // Forward the event to the behavioral class
            dispatchEvent(pEvent);

            if (pEvent->deleteAfterConsume())
            {
                // Remove the consumed event
                delete pEvent;
            }
        }
    }
}

void XFDispatcher::dispatchEvent(const XFEvent * pEvent) const
{
    interface::XFBehavior::TerminateBehavior terminateBehavior;

    terminateBehavior = pEvent->getBehavior()->process(pEvent);

    // Check if behavior should be deleted
    if (terminateBehavior and pEvent->getBehavior()->deleteOnTerminate())
    {
        delete pEvent->getBehavior();
    }
}

#endif // USE_XF_CORE_DISPATCHER_CLASS
