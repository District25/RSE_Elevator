#include "mcu/mcu.h"
#include "critical.h"

static volatile int bOMEnterCriticalRegionNested = 0;

int inISR()
{
	return (__get_IPSR() != 0);
}

void enterCritical()
{
	// Only disable interrupts when not calling from an ISR
	if (!inISR())
	{
		if (!bOMEnterCriticalRegionNested)
		{
			// Turn off the priority configurable interrupts
		    __disable_irq();
		}
		bOMEnterCriticalRegionNested++;
	}
}

void exitCritical()
{
	// Only enable interrupts when not calling from an ISR
	if (!inISR())
	{
		bOMEnterCriticalRegionNested--;

		if (!bOMEnterCriticalRegionNested)
		{
			// Turn on the interrupts with configurable priority
		    __enable_irq();
		}
	}
}
