#ifndef IO_INPUT_H
#define IO_INPUT_H

#include <cstdint>
#include <limits>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "interface/inputcallbackcaller.h"
#include "xf/behavior.h"
#include "event/evwithcounter.h"

namespace io {

/**
 * @brief The Input class provides a notification mechanism on changes of a GPIO input.
 * 
 * The class uses internally interrupts to receive changes from the input pin. Signal
 * debouncing and the notification mechanism is made using a state machine.
 */
class Input : public interface::InputCallbackCaller,
              public XFBehavior
{
    using InputCallbackCaller = interface::InputCallbackCaller;
    using InputCallbackProvider = interface::InputCallbackProvider;
    using CallbackProvider = std::pair<InputCallbackProvider *, InputCallbackProvider::CallbackMethod>;
    using evInput = evWithCounter;

public:
    using InputId = InputCallbackProvider::InputId;

public:
    Input();

    constexpr const static InputId INPUT_ID_MAX = std::numeric_limits<InputId>::max();      ///< Means input identifier not set.

    bool initialize(struct gpio_dt_spec inputSpec, InputId inputId = INPUT_ID_MAX);

    bool isActive() const;      ///< Returns true in case the input is active high.

    inline InputId getInputId() const { return inputIdentifier; }

    // InputCallbackCaller interface implementation
public:
    bool registerCallback(InputCallbackProvider * callbackProvider,
                          InputCallbackProvider::CallbackMethod callbackMethod) override;

    // State machine implementation
protected:
    typedef enum {
        EV_ACTIVE = 1,
        EV_INACTIVE
    } EventId;

    XFEventStatus processEvent() override;

protected:
    bool configure(struct gpio_dt_spec & button, struct gpio_callback & callbackHandlder);

    static void onInputInterrupt_(const struct device * port, struct gpio_callback * cb, gpio_port_pins_t pins);
    void onInputInterrupt();

    void notifyChangedToActive();
    void notifyChangedToInactive();

protected:
    struct gpio_callback buttonCallbackHandler;     /// Note: Must be the first attribute in the class!
    struct gpio_dt_spec inputSpecification;
    InputId inputIdentifier;                        ///< Number to identify input amongst others.
    
    static constexpr const uint8_t MAX_CALLBACKS = 4;
    uint8_t callbacksCount;                                 ///< Stores how many callbacks are registered.
    CallbackProvider callbackProvider[MAX_CALLBACKS];       ///< Registered callback providers.

    evInput evActive;                               ///< Event send when input changes to active.
    evInput evInactive;                             ///< Event send when input changes to inactive.

    bool previousInputState;                        ///< Previous input state for edge detection.
};

} // namespace io
#endif // IO_INPUT_H