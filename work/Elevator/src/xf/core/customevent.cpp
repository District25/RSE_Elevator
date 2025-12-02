#include "xf/customevent.h"

XFCustomEvent::XFCustomEvent(EventId id, bool deleteAfterConsume /* = true */, interface::XFBehavior * pBehavior /* = nullptr */)
    : XFEvent(XFEvent::Event, id),
      bDeleteAfterConsume_(deleteAfterConsume)
{
    setBehavior(pBehavior);
}
