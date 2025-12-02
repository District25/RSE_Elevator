#ifndef BOARD_BUTTONSCONTROLLER_H
#define BOARD_BUTTONSCONTROLLER_H

#include <cstdint>
#include <cassert>
#include "config/buttonscontroller-config.h"
#if !defined(BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD) || defined(BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD) && (BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD == 0)
#include "interface/buttonirq.h"
#include "xf/behavior.h"
#include "interface/buttonscontrollercallbackcaller.h"

namespace board {

/**
 * @brief ButtonsController handles/generates button pressed- and released pressed events.
 *
 * Use the registerCallback() method to get informed about button pressed- and released events.
 */
class ButtonsController : protected interface::ButtonIrq,
                          public interface::ButtonsControllerCallbackCaller,
                          protected XFBehavior
{
public:
    typedef uint8_t tButtonsValue;          ///< Type used to store the state of buttons.
    using CallbackProvider = std::pair<interface::ButtonsControllerCallbackProvider *,
                                       interface::ButtonsControllerCallbackProvider::CallbackMethod>;

    ButtonsController();
    virtual ~ButtonsController();

    bool initialize();

    static ButtonsController & getInstance() { assert(_pInstance); return *_pInstance; }    ///< Access to single instance

    inline void start() { startBehavior(); }

    tButtonsValue readButtonsValue();       ///< @brief Reads the actual state of the buttons.

public:
    void onIrq() override;                  ///< @brief Called by the ISR.

public:
    bool registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                          interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod) override;

public:
    XFEventStatus processEvent() override;

    // Methods executed in state-machine
protected:
    void doCheckButtons();

protected:
    void notifyButtonChange(uint16_t buttonIndex, bool pressed);

protected:
    /**
     * Timeout identifier(s) for this state machine
     */
    typedef enum
    {
        Timeout_DEBOUNCE_id = 10        ///< Timeout id for WAIT
    } eTimeoutId;

    /**
     * Enumeration used to have a unique identifier for every
     * state in the state machine.
     */
    typedef enum
    {
        STATE_UNKOWN = 0,                   ///< Unknown state
        STATE_INITIAL = 1,                  ///< Initial state
        STATE_CHECK_BUTTONS = 2,            ///< Check state of buttons
        STATE_DEBOUNCE = 3                  ///< Debounce state
    } eMainState;

    eMainState _currentState;               ///< Attribute indicating currently active state.

public:
    static constexpr const uint8_t BUTTONS_COUNT = 4;       ///< Amount of buttons available.

protected:
    static ButtonsController * _pInstance;

    tButtonsValue _currentButtonsValue;     ///< Last read buttons state.

    static constexpr const uint8_t MAX_CALLBACKS = 8;
    uint8_t _callbacksCount;                                ///< Stores how many callbacks are registered.
    CallbackProvider _callbackProvider[MAX_CALLBACKS];      ///< Registered callback providers.
};

} // namespace board
#endif // BUTTONSCONTROLLER_EXCLUDE_FROM_BUILD
#endif // BOARD_BUTTONSCONTROLLER_H
