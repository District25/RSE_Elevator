#ifndef LED_BLINKER_H
#define LED_BLINKER_H

#include <cstdint>
#include "xf/behavior.h"
#include "event/evgeneric.h"
#include "board/ledscontroller.h"

namespace led {

/**
 * @brief Class to blink an LED.
 * 
 * The class uses internally the board::LedsController instance to control the assicated LED.
 */
class Blinker : public XFBehavior
{
public:
    Blinker(uint32_t ledIndex);

    void start() { startBehavior(); }

    void blink(uint32_t onTime = 0, uint32_t offTime = 0);
    void stopBlinking();

    bool isBlinking() const;

    // State machine implementation
protected:
    typedef enum {
        EV_BLINK = 1,
        EV_STOP_BLINKING,
        EV_TIMEOUT,
    } EventId;
    
    using evBlink = evGeneric<uint32_t, uint32_t>;
    using evStopBlinking = XFCustomEvent;

    typedef enum
    {
        STATE_UNKOWN = 0,           ///< Unknown state
        STATE_INITIAL = 1,          ///< Initial state
        STATE_LED_OFF = 2,
        STATE_LED_BLINKING = 3,
    } State;


    State currentState;         ///< Indicating currently active state machine state.
    
    XFEventStatus processEvent() override;

protected:
    void updateBlinkTimings(const evBlink * blinkEvent);

protected:
    const uint32_t ledIndex;
    uint32_t onTime;
    uint32_t offTime;
    XFEventHandle timeoutHandle;
    bool active;
};

} // namespace led
#endif // LED_BLINKER_H
