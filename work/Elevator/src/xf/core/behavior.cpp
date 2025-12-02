#include <config/xf-config.h>

#if (USE_XF_CORE_BEHAVIOR_CLASS != 0)

#include <cassert>
#include "xf/initialevent.h"
#include "xf/behavior.h"

XFBehavior::XFBehavior(bool active /* = true */, const char * threadName /* = nullptr */) :
    active(active),
    dispatcher(nullptr),
    deleteOnTerminate_(false),
    pCurrentEvent_(nullptr)
{
    if (active)
    {
        dispatcher = interface::XFDispatcher::create(threadName);
        assert(dispatcher);
    }
}

XFBehavior::~XFBehavior()
{
    if (dispatcher) delete dispatcher;
}

void XFBehavior::startBehavior()
{
    // Send initial event to state machine
    GEN(XFInitialEvent);

    if (dispatcher) dispatcher->start();
}

void XFBehavior::pushEvent(XFEvent * pEvent, uint32_t timeout /* = 0 */)
{
    // Set owner
    pEvent->setBehavior(this);
    // Push to dispatchers event queue
    getDispatcher()->pushEvent(pEvent, timeout);
}

XFEvent::XFEventHandle XFBehavior::pushEvent(int eventId, uint32_t timeout /* = 0 */)
{
     XFEvent * event = new XFEvent(XFEvent::Event, eventId);

    if (event)
    {
        pushEvent(event, timeout);
    }
    return event;
}

void XFBehavior::cancelEvent(XFEventHandle eventHandle)
{
    if (eventHandle)
    {
        XFEvent * const delayedEvent = static_cast<XFEvent *>(eventHandle);
        delayedEvent->stopTimer();
        delete delayedEvent;
    }
}

bool XFBehavior::deleteOnTerminate() const
{
    return deleteOnTerminate_;
}

void XFBehavior::setDeleteOnTerminate(bool deleteBehavior)
{
    deleteOnTerminate_ = deleteBehavior;
}

XFBehavior::TerminateBehavior XFBehavior::process(const XFEvent * pEvent)
{
    XFEventStatus eventStatus;

    setCurrentEvent(pEvent);

    eventStatus = processEvent();

    setCurrentEvent(nullptr);

    return (eventStatus == XFEventStatus::Terminate);
}

interface::XFDispatcher * XFBehavior::getDispatcher()
{
    if (dispatcher)
    {
        return dispatcher;
    }
    return interface::XFDispatcher::getDefaultInstance();
}

void XFBehavior::setCurrentEvent(const XFEvent * pEvent)
{
    pCurrentEvent_ = pEvent;
}

const XFEvent * XFBehavior::getCurrentEvent() const
{
    return pCurrentEvent_;
}

#endif // USE_XF_CORE_BEHAVIOR_CLASS
