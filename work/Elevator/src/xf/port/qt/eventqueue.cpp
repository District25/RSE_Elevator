#include <config/xf-config.h>

#if (USE_XF_PORT_QT_EVENT_QUEUE_CLASS != 0)

#include <cassert>
#include <QtGlobal>
#include <QMutexLocker>
#include "eventqueue.h"

XFEventQueue::XFEventQueue()
{
}

XFEventQueue::~XFEventQueue()
{
    newEvents_.wakeAll();
}

bool XFEventQueue::empty() const
{
    return queue_.isEmpty();
}

bool XFEventQueue::push(const XFEvent * pEvent)
{
    QMutexLocker locker(&mutex_);
    queue_.enqueue(pEvent);
    // Tell waiting thread(s) there is again an event present
    newEvents_.wakeAll();
    return true;
}

const XFEvent * XFEventQueue::front()
{
    return queue_.front();
}

void XFEventQueue::pop()
{
    QMutexLocker locker(&mutex_);
    queue_.dequeue();
}

bool XFEventQueue::pend()
{
    QMutexLocker locker(&mutex_);
    // Wait for new events. Mutex needs to be in lock-state
    // prior to call wait()!
    newEvents_.wait(&mutex_);

    return !queue_.isEmpty();
}

void XFEventQueue::releaseThread()
{
    newEvents_.wakeAll();
}

#endif // USE_XF_PORT_QT_EVENT_QUEUE_CLASS
