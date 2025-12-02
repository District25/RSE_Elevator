#include "events.h"
#include "evbuttonirq.h"

evButtonIrq::evButtonIrq() :
    XFCustomEvent(evButtonIrqId)
{
	setDeleteAfterConsume(false);
}
