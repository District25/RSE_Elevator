#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_EVENT_CLASS != 0)

#include <cassert>
#include <zephyr/kernel.h>
#include "xf/interface/behavior.h"
#include "xf/event.h"

XFEvent::~XFEvent()
{
    stopTimer();
    purgeTimer();
}

uint32_t XFEvent::startTimer(int32_t timeoutInMilliseconds)
{
    struct k_timer * timer;

    // Check if timer was already created
    if (!timeoutHandle)
    {
        // Create timer
        timer = (struct k_timer*)k_malloc(sizeof(struct k_timer));
        timeoutHandle = (uint32_t)timer;

        k_timer_init(timer, (k_timer_expiry_t)&XFEvent::onTimerTimeout, (k_timer_stop_t)&XFEvent::onTimerCanceled);
        k_timer_user_data_set(timer, this);     // Store pointer for later use
    }
    else
    {
        timer = (struct k_timer*)timeoutHandle;
    }
    assert(timer);

    k_timer_start(timer, K_MSEC(timeoutInMilliseconds), K_MSEC(0));

    return timeoutHandle;
}

void XFEvent::stopTimer()
{
    if (timeoutHandle)
    {
        struct k_timer* timer = (struct k_timer*) timeoutHandle;

        k_timer_stop(timer);
    }
}

void XFEvent::purgeTimer()
{
    if (timeoutHandle)
    {
        struct k_timer * timer = (struct k_timer*)timeoutHandle;

        k_free(timer);
        timeoutHandle = 0;
    }
}

// static
void XFEvent::onTimerTimeout(void * timerContext)
{
    struct k_timer * t = (struct k_timer *)timerContext;    // Convert to k_timer
    XFEvent * event = (XFEvent*) k_timer_user_data_get(t);

    event->getBehavior()->pushEvent(event, 0);
}

// static
void XFEvent::onTimerCanceled(void * timerContext)
{
    struct k_timer * t = (struct k_timer *)timerContext;    // Convert to k_timer
    XFEvent * event = (XFEvent*) k_timer_user_data_get(t);

    // Delete software timer
    // Note: Not realy necessary here. For testing purposes only
    event->purgeTimer();
}

#endif // USE_XF_PORT_ZEPHYR_EVENT_CLASS
