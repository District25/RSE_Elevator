#include <cassert>
#include <QCoreApplication>
#include <QTimer>
#include "xf/event.h"
#include "timeoutmanager.h"

XFTimeoutManager * XFTimeoutManager::instance_(nullptr);


// static
XFTimeoutManager & XFTimeoutManager::instance()
{
    static XFTimeoutManager timeoutManager;
    return timeoutManager;
}

void XFTimeoutManager::delayEvent(XFEvent * event, int32_t timeoutInMilliseconds)
{
    QTimer * const timer = event->timeoutHandle;
    assert(timer);

    // QTimer needs to be moved to timeout manager thread
    timer->moveToThread(thread());

    emit sigDelayEvent(timer, timeoutInMilliseconds, event);
}

void XFTimeoutManager::stopTimer(TimeoutHandle timer)
{
    emit sigStopTimer(timer);
}

void XFTimeoutManager::purgeTimer(TimeoutHandle timer)
{
    emit sigPurgeTimer(timer);
}

void XFTimeoutManager::delayEvent_(TimeoutHandle timer, int32_t timeoutInMilliseconds, XFEvent * event)
{
    assert(timer);

    // Configure timer
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval((int)timeoutInMilliseconds);
    timer->setSingleShot(true);

    // Connect callback to XFEvent
    connect(timer, SIGNAL(timeout()), event, SLOT(onTimerTimeout()), Qt::QueuedConnection);

    // Let's go
    timer->start();
}

void XFTimeoutManager::onDelayEvent(TimeoutHandle timer, int32_t timeoutInMilliseconds, XFEvent * event)
{
    delayEvent_(timer, timeoutInMilliseconds, event);
}

void XFTimeoutManager::onStopTimer(TimeoutHandle timer)
{
    if (timer)
    {
        timer->stop();
    }
}

void XFTimeoutManager::onPurgeTimer(TimeoutHandle timer)
{
    if (timer)
    {
        delete timer;
    }
}

XFTimeoutManager::XFTimeoutManager()
{
    assert(!instance_);
    instance_ = this;

    setObjectName("XFTimeoutManager");

    moveToThread(this); // Delegate object to internal thread
}

XFTimeoutManager::~XFTimeoutManager()
{
    QThread::terminate();
    QThread::wait();	// Wait on thread to terminate
}

void XFTimeoutManager::run()
{
    // Stop thread when application gets closed
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(quit()));

    connect(this, SIGNAL(sigDelayEvent(TimeoutHandle,int32_t,XFEvent*)), this, SLOT(onDelayEvent(TimeoutHandle,int32_t,XFEvent*)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigStopTimer(TimeoutHandle)), this, SLOT(onStopTimer(TimeoutHandle)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigPurgeTimer(TimeoutHandle)), this, SLOT(onPurgeTimer(TimeoutHandle)), Qt::QueuedConnection);

    exec();
}
