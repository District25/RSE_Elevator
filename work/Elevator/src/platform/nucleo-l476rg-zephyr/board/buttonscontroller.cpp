#include <string.h>
#include "mcu/mcu.h"
#include "trace/trace.h"
#include "hal/buttons.h"
#include "buttonscontroller.h"

#if !defined(BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD) || defined(BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD) && (BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD == 0)

#ifndef BUTTONSCONTROLLER_DEBOUNCE_TIME
    #define BUTTONSCONTROLLER_DEBOUNCE_TIME    50   /* in milliseconds */
#endif

#include "event/evbuttonirq.h"

namespace board {

ButtonsController * ButtonsController::_pInstance(nullptr);

ButtonsController::ButtonsController()
{
    assert(!_pInstance);        // Singleton pattern. Only one instance allowed.
    _pInstance = this;          // Store 'this' to static pointer

    _currentState = STATE_UNKOWN;

    _callbacksCount = 0;
}

ButtonsController::~ButtonsController()
{

}

bool ButtonsController::initialize()
{
    // Now the actual state of the buttons can be read
    _currentButtonsValue = readButtonsValue();

    return true;
}

void ButtonsController::onIrq()
{
    static evButtonIrq evButtonIrq;

    pushEvent(&evButtonIrq, true);
}

bool ButtonsController::registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                                         interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod)
{
    if (_callbacksCount < MAX_CALLBACKS)
    {
        for (uint8_t i = _callbacksCount; i < MAX_CALLBACKS; i++)
        {
            if (_callbackProvider[i].first == nullptr)
            {
                _callbackProvider[i].first = callbackProvider;
                _callbackProvider[i].second = callbackMethod;
                _callbacksCount++;
                break;
            }
        }
        return true;
    }
    return false;
}

ButtonsController::tButtonsValue ButtonsController::readButtonsValue()
{
    tButtonsValue value = 0;

    // Button BTN0
    value |= (hal::buttons::isButtonPressed(0)) ? 0x01 : 0x00;    

    // Button BTN1
    value |= (hal::buttons::isButtonPressed(1)) ? 0x02 : 0x00; 

    // Button BTN2
    value |= (hal::buttons::isButtonPressed(2)) ? 0x04 : 0x00; 

    // Button BTN3
    value |= (hal::buttons::isButtonPressed(3)) ? 0x08 : 0x00; 

    return value;
}

XFEventStatus ButtonsController::processEvent()
{
    eMainState newState = _currentState;

    // Handle transition changes
    switch (_currentState)
    {
    case STATE_UNKOWN:
        // Decouple caller of startBehavior() with
        // the transition from UNKNOWN to INITIAL.
        if (getCurrentEvent()->getEventType() == XFEvent::Initial)
        {
            newState = STATE_INITIAL;
        }
        break;
    case STATE_INITIAL:
        if (getCurrentEvent()->getEventType() == XFEvent::DefaultTransition)
        {
            newState = STATE_CHECK_BUTTONS;
        }
        break;
    case STATE_CHECK_BUTTONS:
        if (getCurrentEvent()->getEventType() == XFEvent::Event &&
            getCurrentEvent()->getId() == evButtonIrqId)
        {
            newState = STATE_DEBOUNCE;
        }
        break;
    case STATE_DEBOUNCE:
        if (getCurrentEvent()->getEventType() == XFEvent::Event &&
            getCurrentEvent()->getId() == Timeout_DEBOUNCE_id)
        {
            newState = STATE_CHECK_BUTTONS;
        }
        break;
    }

    // Handle transitions
    if (_currentState != newState)
    {
        switch (newState)
        {
        case STATE_INITIAL:
            GEN(XFDefaultTransition);
            break;
        case STATE_CHECK_BUTTONS:

            doCheckButtons();

            break;
        case STATE_DEBOUNCE:
            pushEvent(Timeout_DEBOUNCE_id, BUTTONSCONTROLLER_DEBOUNCE_TIME);
            break;
        default:
            break;
        }

        _currentState = newState;
    }
    return XFEventStatus::Consumed;
}


void ButtonsController::doCheckButtons()
{
    tButtonsValue newButtonsValue = readButtonsValue();
    uint8_t mask = 0x01;

    if (_currentButtonsValue != newButtonsValue)
    {
        for (uint32_t i = 0; i < BUTTONS_COUNT; i++)
        {
            if ((_currentButtonsValue & mask) != (newButtonsValue & mask))
            {
                notifyButtonChange(i, (newButtonsValue & mask));
            }

            mask <<= 0x01;
        }

        _currentButtonsValue = newButtonsValue;
    }
}

void ButtonsController::notifyButtonChange(uint16_t buttonIndex, bool pressed)
{
#if (TRACE_BUTTONSCONTROLLER_NOTIFICATIONS != 0)
    Trace::out("Button %d %s", buttonIndex, (pressed) ? "pressed" : "released");
#endif

    for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
    {
        if (_callbackProvider[i].first != nullptr)
        {
            (_callbackProvider[i].first->*_callbackProvider[i].second)(buttonIndex, pressed);
        }
        else
        {
            break;
        }
    }
}

} // namespace board
#endif // BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD
