#ifndef ELEVATOR_CONTROLLER_H
#define ELEVATOR_CONTROLLER_H

#include "xf/behavior.h"
#include "event/evgeneric.h"
#include "interface/buttonscontrollercallbackprovider.h"
#include "interface/inputcallbackprovider.h"
#include "interface/elevatorcontroller.h"
#include "interface/elevatorcontrollerobserver.h"
#include "led/blinker.h"
#include "button.h"
#include "security/issuenotifier.h"

namespace elevator {

/**
 * @brief The elevator::Controller moves the elevator from one floor into another.
 * 
 * Requirements:
 * - Needs to know in which floor the elevator actually is
 * - Needs to know if elevator is moving or not
 */
class Controller : protected XFBehavior,
                   protected interface::ButtonsControllerCallbackProvider,
                   protected interface::InputCallbackProvider,
                   protected ::interface::ElevatorController,
                   protected ::interface::SecurityIssueObserver
{
    friend class Button;
    
    using ElevatorControllerObserver = interface::ElevatorControllerObserver;
    using SecurityIssueObserver = interface::SecurityIssueObserver;
public:
    Controller();

    bool initialize(io::Input & limitSwitchFloor1, io::Input & limitSwitchFloor2);
    void start();

public:
    typedef enum FLOOR_NUMBER {
        FN_UNKNOWN = -1,
        FN_FLOOR0  = 0,
        FN_FLOOR1,
        FN_FLOOR_COUNT,     // Note: Must always be the last element in the enumeration!
    } FLOOR_NUMBER;

    typedef enum ELEVATOR_STATE {
        EL_STOPPED,
        EL_MOVING_UP,
        EL_MOVING_DOWN
    } ELEVATOR_STATE;

    typedef enum ELEVATOR_ERROR_NUMBER {
        EE_NO_ERROR = 0,
        EE_INITIAL_ELEVATOR_POSITION,   // Actual position of elevator unknown at initialisation time.
        EE_SECURITY_ISSUE,              // Security monitor detected a security issue.
    } ELEVATOR_ERROR_NUMBER;

    FloorNumber getCurrentFloorNumber() const { return (FloorNumber)currentFloorNumber; }
    bool isElevatorOperatinal() const { return (currentState == STATE_ELEVATOR_STOPPED or 
                                                currentState == STATE_ELEVATOR_MOVING); }
    bool isElevatorMoving() const { return elevatorState != EL_STOPPED; }
    FloorNumber getDestinationFloorNumber() const { return (FloorNumber)destinationFloorNumber; }

    // State machine implementation
protected:
    typedef enum EventId {
        EV_MOTOR_UP = 1,
        EV_MOTOR_DOWN,
        EV_MOTOR_STOP,
        EV_LIMIT_SWITCH_CHANGED,
        EV_REQUEST_ELEVATOR_FLOOR,      ///< Request elevator to go/come to floor x.
        EV_SECURITY_ISSUE,
    } EventId;
    
    using evLimitSwitchChanged = evGeneric<uint32_t, bool>;
    using evRequestElevatorOnFloor = evGeneric<FloorNumber>;
    using evSecurityIssue = evGeneric<IssueNumber>;

    typedef enum State
    {
        STATE_UNKOWN = 0,           ///< Unknown state
        STATE_INITIAL = 1,          ///< Initial state
        STATE_ELEVATOR_STOPPED = 2, ///< Elevator stopped
        STATE_ELEVATOR_MOVING,      ///< Elevator moving to another floor
        STATE_ERROR,                ///< Error state
    } State;

    XFEventStatus processEvent() override;

    State currentState;         ///< Indicating currently active state machine state.

    // Interface ButtonsControllerCallbackProvider implementation
protected:
    void onButtonPressed(uint16_t buttonIndex, bool pressed);

    // Callback method for limit switchs
protected:
    void onLimitSwitchChanged(io::Input::InputId inputId, bool active);

    // Callback method for the elevator buttons
protected:
    bool onRequestGoToFloor(FloorNumber floorNumber);

    // ElevatorController interface implementation
public:
    bool subscribe(ElevatorControllerObserver * observer) override;         ///< Registers a new observer. Returns true on success.
    void unsubscribe(ElevatorControllerObserver * observer) override;       ///< Un-subscribes a registered observer.

protected:
    void notifyElevatorStarted() override;                                  ///< Notifies observers that elevator started to move to another floor.
    void notifyElevatorReachedFloor(FloorNumber floorNumber) override;      ///< Notifies observers about elevator reached destination floor.
    void notifyElevatorError() override;                                    ///< Notifies observers that elevator has an error.

    // SecurityIssueObserver interface implementation
protected:
    void onSecurityIssue(IssueNumber issueNumber);

protected:
    void updateElevatorInFloorLed(uint32_t floorIndex, bool enable = true) const;
    void updateElevatorInFloorLeds() const;
    bool evaluteFloorNumber(bool tracePosition = false);

protected:
    FLOOR_NUMBER currentFloorNumber;        ///< Elevators actual floor number. FN_UNKNOWN when moving.
    FLOOR_NUMBER destinationFloorNumber;    ///< Floor number to which elevator is moving to. FN_UNKNOWN when stopped.
    ELEVATOR_STATE elevatorState;           ///< Elevators actual state (state machine attribute).
    ELEVATOR_ERROR_NUMBER errorNumber;      ///< Stores last known error.
    led::Blinker errorLed;                  ///< Blinker for error LED.
    led::Blinker operationalLed;            ///< Blinker for operational LED.

    io::Input * limitSwitchFloor[2];

protected:
    static constexpr const uint8_t MAX_OBSERVERS = 8;                   ///< Maximum allowed observers.
    uint8_t observerCount;                                              ///< Stores how many observers are subscribed.
    interface::ElevatorControllerObserver * observer[MAX_OBSERVERS];    ///< Array holding subscribed observers.
};

} // namespace elevator

#endif // ELEVATOR_CONTROLLER_H