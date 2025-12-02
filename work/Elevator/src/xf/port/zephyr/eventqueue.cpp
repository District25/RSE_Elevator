#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_EVENT_QUEUE_CLASS != 0)

#include <cassert>
#include "xf/event.h"
#include "eventqueue.h"

XFEventQueue::XFEventQueue()
{
    k_msgq_init(&queue_, queue_buffer, sizeof(XFEvent*), 10);
    k_condvar_init(&waitForEvent);
    k_mutex_init(&waitForEventMutex);
}

XFEventQueue::~XFEventQueue()
{
	k_msgq_cleanup(&queue_);
}

bool XFEventQueue::empty() const
{
    const uint32_t count = k_msgq_num_used_get(const_cast<EventQueue*>(&queue_)); 	
    return (count == 0) ? true : false;
}

bool XFEventQueue::push(const XFEvent * pEvent)
{
    int status = ENOMSG;

    if (pEvent->getDelay() == 0)
    {
        status = k_msgq_put(&queue_, &pEvent, K_NO_WAIT);
        if (!k_is_in_isr())
            k_mutex_lock(&waitForEventMutex, K_FOREVER);
        k_condvar_signal(&waitForEvent);
        if (!k_is_in_isr())
            k_mutex_unlock(&waitForEventMutex);
    }

    return (status == 0) ? true : false;
}

const XFEvent * XFEventQueue::front()
{
    XFEvent * event = nullptr;

    k_msgq_peek(&queue_, &event);
    return event;
}

void XFEventQueue::pop()
{
    XFEvent * event = nullptr;
    k_msgq_get(&queue_, &event, K_NO_WAIT);
}

bool XFEventQueue::pend()
{
    // Wait until an event is put into the queue
    k_mutex_lock(&waitForEventMutex, K_FOREVER);
    k_condvar_wait(&waitForEvent, &waitForEventMutex, K_FOREVER);
//k_sleep(K_MSEC(1));
    k_mutex_unlock(&waitForEventMutex);
    return true;
}

#endif // USE_XF_PORT_ZEPHYR_EVENT_QUEUE_CLASS
