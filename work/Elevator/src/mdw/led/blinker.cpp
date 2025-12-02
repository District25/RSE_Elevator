#include "blinker.h"

namespace led {

Blinker::Blinker(uint32_t ledIndex) :
    XFBehavior(/* active = */ false),
    currentState(STATE_INITIAL),
    ledIndex(ledIndex),
    onTime(0),
    offTime(0),
    timeoutHandle(0),
    active(false)
{
}

void Blinker::blink(uint32_t onTime /* = 0 */, uint32_t offTime /*  = 0 */)
{
    GEN(evBlink(EV_BLINK, onTime, offTime));
}

void Blinker::stopBlinking()
{
    GEN(evStopBlinking(EV_STOP_BLINKING));
}

bool Blinker::isBlinking() const
{
    return (currentState == STATE_LED_BLINKING);
}

XFEventStatus Blinker::processEvent()
{
    // Helper define to change state during transition
    #define TRANSIT_TO(state) newState = (state); inTransition = true;

    const XFEvent * const event = getCurrentEvent();
    bool inTransition = false;
    auto newState = currentState;

    // Handle transition changes
    switch (currentState)
    {
    case STATE_UNKOWN:
    case STATE_INITIAL:
        TRANSIT_TO(STATE_LED_OFF);
        break;
    case STATE_LED_OFF:
        if (event->getId() == EV_BLINK)
        {
            updateBlinkTimings(static_cast<const evBlink*>(event));

            TRANSIT_TO(STATE_LED_BLINKING);
        }
        break;
    case STATE_LED_BLINKING:
        switch (event->getId())
        {
        case EV_TIMEOUT:
            TRANSIT_TO(STATE_LED_BLINKING);
            break;
        case EV_STOP_BLINKING:
            TRANSIT_TO(STATE_LED_OFF);
            break;
        case EV_BLINK:
            updateBlinkTimings(static_cast<const evBlink*>(event));
            TRANSIT_TO(STATE_LED_BLINKING);     // Re-enter state
            break;
        default:
            break;
        }
        break;
    }

    // Handle transitions
    if (inTransition)
    {
        switch (newState)
        {
        case STATE_LED_OFF:
            board::LedsController::getInstance().setLed(ledIndex, false);
            active = false;

            if (timeoutHandle)
            {
                cancelEvent(timeoutHandle);
                timeoutHandle = 0;
            }

            break;
        case STATE_LED_BLINKING:
            switch (event->getId())
            {
            case EV_BLINK:
                if (timeoutHandle)  // Cancel any running timeout
                {
                    cancelEvent(timeoutHandle);
                    timeoutHandle = 0;
                }
                board::LedsController::getInstance().setLed(ledIndex);
                active = true;
                timeoutHandle = pushEvent(EV_TIMEOUT, onTime);
                break;
            case EV_TIMEOUT:
                if (active)
                {
                    board::LedsController::getInstance().setLed(ledIndex);
                    timeoutHandle = pushEvent(EV_TIMEOUT, onTime);
                }
                else
                {
                    board::LedsController::getInstance().setLed(ledIndex, false);
                    timeoutHandle = pushEvent(EV_TIMEOUT, offTime);
                }
                active = !active;

                break;
            }
        default:
            break;
        }

        currentState = newState;
    }

    return XFEventStatus::Consumed;
}

void Blinker::updateBlinkTimings(const evBlink * blinkEvent)
{
    onTime = blinkEvent->attribute1;
    offTime = blinkEvent->attribute2;
}

} // namespace led
