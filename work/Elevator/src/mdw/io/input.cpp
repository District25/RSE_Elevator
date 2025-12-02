#include <cassert>
#include "trace/trace.h"
#include "xf/xf.h"
#include "input.h"

namespace io {

Input::Input():
    XFBehavior(/* active = */ false),
    inputSpecification {},
    inputIdentifier(INPUT_ID_MAX),
    callbacksCount(0),
    callbackProvider {},
    evActive(EV_ACTIVE, false, this),
    evInactive(EV_INACTIVE, false, this),
    previousInputState(false)
{
}

bool Input::initialize(struct gpio_dt_spec inputSpec, InputId inputId /* = INPUT_ID_MAX */)
{
    bool success;
    assert(inputSpecification.port == nullptr);     // Initialization method should only be called once!

    inputSpecification = inputSpec;
    inputIdentifier = inputId;

    success = configure(inputSpecification, buttonCallbackHandler);

    // GPIO is now conigured. Request actual state
    previousInputState = isActive();    // Memorise actual input state
    
    return success;
}

bool Input::isActive() const
{
    return gpio_pin_get_dt(&inputSpecification);
}

bool Input::registerCallback(InputCallbackProvider * callbackProvider, 
                             InputCallbackProvider::CallbackMethod callbackMethod)
{
    if (callbacksCount < MAX_CALLBACKS)
    {
        for (uint8_t i = callbacksCount; i < MAX_CALLBACKS; i++)
        {
            if (this->callbackProvider[i].first == nullptr)
            {
                this->callbackProvider[i].first = callbackProvider;
                this->callbackProvider[i].second = callbackMethod;
                callbacksCount++;
                break;
            }
        }
        return true;
    }
    return false;
}

XFEventStatus Input::processEvent()
{
    if (getCurrentEvent()->getId() == EV_ACTIVE)
    {
        // Event got processed. Decrement counter
        evActive.decrementCounter();
    }
    else if (getCurrentEvent()->getId() == EV_INACTIVE)
    {
        evInactive.decrementCounter();
    }

    const bool actualInputState = isActive();

    if (actualInputState != previousInputState)
    {
        if (isActive())
        {
            notifyChangedToActive();
        }
        else
        {
            notifyChangedToInactive();
        }

        previousInputState = actualInputState;
    }

    return XFEventStatus::Consumed;
}

bool Input::configure(struct gpio_dt_spec & input, struct gpio_callback & callbackHandlder)
{
    int ret;

    if (!gpio_is_ready_dt(&input))
    {
        return false;
    }

    // Configure input gpio as input
    ret = gpio_pin_configure_dt(&input, GPIO_INPUT);
    if (ret != 0)
    {
        Trace::out("Error %d: Failed to configure %s pin %d", ret, input.port->name, input.pin);
        return false;
    }

    // Enable interrupt on input for rising edge
    ret = gpio_pin_interrupt_configure_dt(&input, GPIO_INT_EDGE_BOTH);
    if (ret != 0)
    {
        Trace::out("Error %d: Failed to configure interrupt on %s pin %d", ret, input.port->name, input.pin);
        return false;
    }

    // Initialize callback structure for input interrupt
    gpio_init_callback(&callbackHandlder, onInputInterrupt_, BIT(input.pin));

    // Attach callback function to input interrupt
    gpio_add_callback_dt(&input, &callbackHandlder);

    return true;
}

/** 
 * @brief Callback function called upon interrupt.
 * 
 * Every input interrupt gets muxed into this function (common callback handler).
 */
void Input::onInputInterrupt_(const struct device * port, struct gpio_callback * cb, gpio_port_pins_t pins)
{
    // Unfortunately, the gpio callback does not provide some user data or context to be added.
    // Hack to get pointer to the Input object: 
    Input * const me = (Input *)((uint32_t *)cb - (sizeof(XFBehavior) / 4) - 1);

    me->onInputInterrupt();
}

void Input::onInputInterrupt()
{
    if (!inputSpecification.port or !XF::isRunning())
    {
        return;
    }

    if (isActive())
    {
        // Increment event counter and push event if not already one got pushed
        evActive.incrementCounter();
        if (evActive.getCounter() == 1)
        {
            pushEvent(&evActive, 10);       // Delay processing of event: GPIO debouncing
        }
        else
        {   
            evActive.decrementCounter();
        }
    }
    else
    {
        evInactive.incrementCounter();
        if (evInactive.getCounter() == 1)
        {
            pushEvent(&evInactive, 10);     // Delay processing of event: GPIO debouncing
        }
        else
        {   
            evInactive.decrementCounter();
        }
    }
}

void Input::notifyChangedToActive()
{
    //Trace::out("Input %u active", inputIdentifier);

    for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
    {
        if (callbackProvider[i].first != nullptr)
        {
            (callbackProvider[i].first->*callbackProvider[i].second)(getInputId(), true);
        }
        else
        {
            break;
        }
    }
}

void Input::notifyChangedToInactive()
{
    //Trace::out("Input %u inactive", inputIdentifier);

    for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
    {
        if (callbackProvider[i].first != nullptr)
        {
            (callbackProvider[i].first->*callbackProvider[i].second)(getInputId(), false);
        }
        else
        {
            break;
        }
    }
}

} // namespace io
