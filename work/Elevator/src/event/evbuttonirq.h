#ifndef EVBUTTONIRQ_H
#define EVBUTTONIRQ_H

#include "xf/customevent.h"
#include "events.h"

/**
 * @brief Event used in GPIO ISR (interrupt service routine).
 *
 * Is send on rising and falling edge of the signal.
 */
class evButtonIrq : public XFCustomEvent
{
public:
	evButtonIrq();
};

#endif // EVBUTTONIRQ_H
