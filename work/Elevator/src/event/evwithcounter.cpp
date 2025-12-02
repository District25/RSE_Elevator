#include "evwithcounter.h"

evWithCounter::evWithCounter(EventId id, bool deleteAfterConsume /* = true */, interface::XFBehavior * pBehavior /* = nullptr */):
 XFCustomEvent(id, deleteAfterConsume, pBehavior)
{}

void evWithCounter::incrementCounter()
{
    ++counter;
}

void evWithCounter::decrementCounter()
{
    --counter;
}

uint32_t evWithCounter::getCounter()
{
    return counter;
}
