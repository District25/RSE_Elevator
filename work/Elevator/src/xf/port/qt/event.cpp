#include <config/xf-config.h>

#if (USE_XF_PORT_QT_EVENT_CLASS != 0)

#include <cassert>
#include <QTimer>
#include "timeoutmanager.h"
#include "xf/interface/behavior.h"
#include "xf/event.h"

XFEvent::~XFEvent()
{
    stopTimer();
    purgeTimer();
}

TimeoutHandle XFEvent::startTimer(int32_t timeoutInMilliseconds)
{
    // Check if timer was not already created
    assert(timeoutHandle == nullptr);

    // Note: Event needs to be moved to timeout manager thread, otherwise delayed event will not be correctly handled
    moveToThread(XFTimeoutManager::instance().thread());

    //qDebug("%s: %p", "App thread", QCoreApplication::instance()->thread());
    //qDebug("%s: %p", "XFTimeoutManager thread", &XFTimeoutManager::instance());
    //qDebug("%s: %p", "XFTimeoutManagers QObject thread", XFTimeoutManager::instance().thread());

    // QTimer needs to be created here (for reference)
    timeoutHandle = new QTimer;
    // Delegate timer handling to timeout manager
    XFTimeoutManager::instance().delayEvent(this, timeoutInMilliseconds);

    return timeoutHandle;
}

void XFEvent::stopTimer()
{
    if (timeoutHandle)
    {
        XFTimeoutManager::instance().stopTimer(timeoutHandle);
    }
}

void XFEvent::purgeTimer()
{
    if (timeoutHandle)
    {
        XFTimeoutManager::instance().purgeTimer(timeoutHandle);
        timeoutHandle = 0;
    }
}

void XFEvent::onTimerTimeout()
{
    //qDebug("XFEvent::onTimerTimeout()");
    getBehavior()->pushEvent(this, 0);
}

#endif // USE_XF_PORT_QT_EVENT_CLASS
