#include "board/buttonscontroller.h"
#include "motor/driver.h"
#include "io/input.h"
#include "trace/trace.h"
#include "controller.h"

namespace elevator {

Controller::Controller():
    XFBehavior(/* active = */ true),
    currentState(STATE_INITIAL),
    currentFloorNumber(FN_UNKNOWN),
    destinationFloorNumber(FN_UNKNOWN),
    elevatorState(EL_STOPPED),
    errorNumber(EE_NO_ERROR),
    errorLed(6),
    operationalLed(7),
    limitSwitchFloor {},
    observerCount(0),
    observer {}
{
}

bool Controller::initialize(io::Input & limitSwitchFloor1, io::Input & limitSwitchFloor2)
{
    bool success = true;
    success &= board::ButtonsController::getInstance().registerCallback(this, (ButtonsControllerCallbackProvider::CallbackMethod)&Controller::onButtonPressed);
    assert(true);

    limitSwitchFloor[0] = &limitSwitchFloor1;
    limitSwitchFloor[1] = &limitSwitchFloor2;

    success &= limitSwitchFloor1.registerCallback(this, (InputCallbackProvider::CallbackMethod)&Controller::onLimitSwitchChanged);
    assert(true);

    success &= limitSwitchFloor2.registerCallback(this, (InputCallbackProvider::CallbackMethod)&Controller::onLimitSwitchChanged);
    assert(true);

    success &= SecurityIssueObserver::subscribeToNotifier();
    assert(true);

    return success;
}

void Controller::start()
{
    errorLed.start();
    operationalLed.start();
    startBehavior();
}

XFEventStatus Controller::processEvent()
{
    // Helper define to change state during transition
    #define TRANSIT_TO(state) newState = (state); inTransition = true;

    const XFEvent * event = getCurrentEvent();
    bool inTransition = false;
    auto newState = currentState;

    // Superior event handling (independent of actual state)
    switch (event->getId())
    {
    case EV_LIMIT_SWITCH_CHANGED:
        {
            const evLimitSwitchChanged * evLimitSwitch = static_cast<const evLimitSwitchChanged *>(event);
            const uint32_t ledsInFloorOffset = 0;
            const uint32_t floorIndex = ledsInFloorOffset + evLimitSwitch->attribute1;
            const bool enableLed = evLimitSwitch->attribute2;

            // Enabled / disable LEDs indicating elevator in floor present:
            // led-in-floor-1
            // led-in-floor-2
            updateElevatorInFloorLed(floorIndex, enableLed);
        }
        break;
    case EV_SECURITY_ISSUE:
        {
            errorNumber = EE_SECURITY_ISSUE;
            errorLed.blink(60, 100);
            
            TRANSIT_TO(STATE_ERROR);
        }
        break;
    }

    // Handle transition changes
    switch (currentState)
    {
    case STATE_UNKOWN:
    case STATE_INITIAL:
        if (event->getEventType() == XFEvent::Initial or
            (event->getEventType() == XFEvent::Event and event->getId() == EV_LIMIT_SWITCH_CHANGED)
           )
        {
            updateElevatorInFloorLeds();
            if (evaluteFloorNumber(true))
            {
                TRANSIT_TO(STATE_ELEVATOR_STOPPED);
            }
            else
            {
                // Position of elevator unknown
                errorNumber = EE_INITIAL_ELEVATOR_POSITION;
                errorLed.blink(60, 300);
            }
        }
        break;
    case STATE_ELEVATOR_STOPPED:
        if (event->getId() == EV_REQUEST_ELEVATOR_FLOOR)
        {
            const evRequestElevatorOnFloor * evRequestFloor = static_cast<const evRequestElevatorOnFloor *>(event);
            const FloorNumber requestedFloorNumber = evRequestFloor->attribute1;

            if (!isElevatorMoving())
            {
                if (requestedFloorNumber != getCurrentFloorNumber())
                {
                    const bool moveElevatorUp = (requestedFloorNumber > getCurrentFloorNumber()) ? true : false;

                    Trace::out("Elevator moving %s...", (moveElevatorUp) ? "up" : "down");

                    // Start to move elevator
                    motor::Driver::getInstance().enableMotor((moveElevatorUp) ? motor::Driver::FORWARD : motor::Driver::BACKWARD);

                    // Update floor number and elevator state attributes
                    destinationFloorNumber = (FLOOR_NUMBER)requestedFloorNumber;
                    currentFloorNumber = FN_UNKNOWN;
                    elevatorState = (moveElevatorUp) ? EL_MOVING_UP : EL_MOVING_DOWN;

                    // Notify observers
                    notifyElevatorStarted();

                    TRANSIT_TO(STATE_ELEVATOR_MOVING);
                }
            }
        }
        break;
    case STATE_ELEVATOR_MOVING:
        {
            switch (event->getId())
            {
            case EV_LIMIT_SWITCH_CHANGED:
                {
                    const evLimitSwitchChanged * evLimitSwitch = static_cast<const evLimitSwitchChanged *>(event);
                    const uint32_t inputId = evLimitSwitch->attribute1;
                    const bool limitSwitchActive = evLimitSwitch->attribute2;

                    if (limitSwitchActive)
                    {
                        Trace::out("Elevator reached floor %u", inputId + 1);

                        motor::Driver::getInstance().disableMotor();

                        // Update floor number and elevator state attributes
                        destinationFloorNumber = FN_UNKNOWN;
                        currentFloorNumber = (FLOOR_NUMBER)inputId;
                        elevatorState = EL_STOPPED;

                        // Notify observers
                        notifyElevatorReachedFloor(currentFloorNumber);

                        TRANSIT_TO(STATE_ELEVATOR_STOPPED);
                    }
                }
            }
        }
        break;
    case STATE_ERROR:
        // Do nothing
        break;
    }

    // Handle actions on entry
    if (inTransition)
    {
        switch (newState)
        {
        case STATE_ELEVATOR_STOPPED:
            motor::Driver::getInstance().disableMotor();
            errorLed.stopBlinking();
            operationalLed.blink(50, 950);
            Trace::out("Elevator ready");
            break;
        case STATE_ELEVATOR_MOVING:
            operationalLed.blink(50, 100);
            break;
        case STATE_ERROR:
            operationalLed.stopBlinking();
            Trace::out("Elevator error!");
            notifyElevatorError();
            break;
        default:
            break;
        }

        currentState = newState;
    }

    // Events in state
    switch (currentState)
    {
    case STATE_INITIAL:
        if (event->getId() == EV_MOTOR_UP)
        {
            motor::Driver::getInstance().enableMotor(motor::Driver::BACKWARD);
        }
        else if (event->getId() == EV_MOTOR_DOWN)
        {
            motor::Driver::getInstance().enableMotor(motor::Driver::FORWARD);
        }
        else if (event->getId() == EV_MOTOR_STOP)
        {
            motor::Driver::getInstance().disableMotor();
        }
        break;
    default:
        break;
    }

    return XFEventStatus::Consumed;
}

void Controller::onButtonPressed(uint16_t buttonIndex, bool pressed)
{
    // Convert button index to button name (for readability)
    const ButtonName button = (ButtonName)buttonIndex;

    switch (button)
    {
    case B_CALL_ON_FLOOR_1:
        // no break
    case B_ELEVATOR_DOWN:
        {
            if (currentState == STATE_INITIAL)
            {
                // Allow to move elevator to a precise floor
                pushEvent((pressed) ? EV_MOTOR_UP : EV_MOTOR_STOP);
            }
        }
        break;
    case B_CALL_ON_FLOOR_2:
        // no break
    case B_ELEVATOR_UP:
        {
            if (currentState == STATE_INITIAL)
            {
                // Allow to move elevator to a precise floor
                pushEvent((pressed) ? EV_MOTOR_DOWN : EV_MOTOR_STOP);
            }
        }
        break;
    default:
        break;
    }
}

void Controller::onLimitSwitchChanged(io::Input::InputId inputId, bool active)
{
    Trace::out("Limit switch %u %s", inputId, (active) ? "activated" : "desactivated");
    GEN(evLimitSwitchChanged(EV_LIMIT_SWITCH_CHANGED, inputId, active));
}

bool Controller::onRequestGoToFloor(FloorNumber floorNumber)
{
    bool success = false;

    Trace::out("Request to go to floor %u", floorNumber + 1);

    if (floorNumber < FN_FLOOR_COUNT)   // Floor number must be valid
    {
        if (floorNumber != getCurrentFloorNumber()) // Not already in requested floor
        {
            GEN(evRequestElevatorOnFloor(EV_REQUEST_ELEVATOR_FLOOR, floorNumber));
            success = true;
        }
    }
    return success;
}

bool Controller::subscribe(ElevatorControllerObserver * observer)
{
    if (observer != nullptr and observerCount < MAX_OBSERVERS)
    {
        for (int i = observerCount; i < MAX_OBSERVERS; i++)
        {
            if (this->observer[i] == nullptr)
            {
                this->observer[i] = observer;
                observerCount++;
                return true;
            }
        }
    }
    return false;
}

void Controller::unsubscribe(ElevatorControllerObserver * observer)
{
    if (observerCount > 0 and observer != nullptr)
    {
        for (int i = 0; i < MAX_OBSERVERS; i++)
        {
            if (this->observer[i] == observer)
            {
                this->observer[i] = nullptr;
                observerCount--;
                break;
            }
        }
    }
}

void Controller::notifyElevatorStarted()
{
    for (int i = 0; i < MAX_OBSERVERS; i++)
    {
        if (observer[i] != nullptr)
        {
            observer[i]->onElevatorStarted();
        }
    }
}

void Controller::notifyElevatorReachedFloor(FloorNumber floorNumber)
{
    for (int i = 0; i < MAX_OBSERVERS; i++)
    {
        if (observer[i] != nullptr)
        {
            observer[i]->onElevatorReachedFloor(floorNumber);
        }
    }
}

void Controller::notifyElevatorError()
{
    for (int i = 0; i < MAX_OBSERVERS; i++)
    {
        if (observer[i] != nullptr)
        {
            observer[i]->onElevatorError();
        }
    }
}

void Controller::onSecurityIssue(IssueNumber issueNumber)
{
    motor::Driver::getInstance().disableMotor();
    
    Trace::out("Controller: Received a security issue: %lu", (uint32_t)issueNumber);

    GEN(evSecurityIssue(EV_SECURITY_ISSUE, issueNumber));
}

void Controller::updateElevatorInFloorLed(uint32_t floorIndex, bool enable /* = true */) const
{
    const uint32_t ledsInFloorOffset = 0;
    if (floorIndex < (uint32_t)FN_FLOOR_COUNT)
    {
        board::LedsController::getInstance().setLed(ledsInFloorOffset + floorIndex, enable);
    }
}

void Controller::updateElevatorInFloorLeds() const
{
    for (uint32_t i = FN_FLOOR0; i < FN_FLOOR_COUNT; i++)
    {
        updateElevatorInFloorLed(i, limitSwitchFloor[i]->isActive());
    }
}

bool Controller::evaluteFloorNumber(bool tracePosition /* = false */)
{
    bool success = true;

    if (limitSwitchFloor[0]->isActive())
    {
        currentFloorNumber = FN_FLOOR0;
    }
    else if (limitSwitchFloor[1]->isActive())
    {
        currentFloorNumber = FN_FLOOR1;
    }
    else
    {
        currentFloorNumber = FN_UNKNOWN;
        success = false;
    }

    if (tracePosition)
    {
        switch (currentFloorNumber)
        {
        case FN_FLOOR0:
            Trace::out("Elevator is in %s", "floor 1");
            break;
        case FN_FLOOR1:
            Trace::out("Elevator is in %s", "floor 2");
            break;
        case FN_UNKNOWN:
            Trace::out("Warning; Elevator position unknown");
            break;
        case FN_FLOOR_COUNT: break;
        }
    }

    return success;
}

} // namespace elevator
