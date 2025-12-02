#ifndef EV_WITH_COUNTER_H
#define EV_WITH_COUNTER_H

#include "xf/customevent.h"
#include "lock/atomic.hpp"

/**
 * @brief Event with a counter attribute.
 * 
 * The counter can be atomically incremented and decremented using according methods.
 * It may be useful if you have an event arising often, but you do not want to overflow 
 * the event queue with this event.
 */
class evWithCounter : public XFCustomEvent
{
public:
    explicit evWithCounter(EventId id, bool deleteAfterConsume = true, interface::XFBehavior * pBehavior = nullptr);

    void incrementCounter();
    void decrementCounter();
    uint32_t getCounter();

protected:
    Atomic<> counter;       ///< Counter used for inc- and decrementation.
};

#endif // EV_WITH_COUNTER_H
