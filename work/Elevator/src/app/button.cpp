
#include "board/buttonscontroller.h"
#include "board/ledscontroller.h"
#include "controller.h"
#include "button.h"

namespace elevator {
    
Button::Button(FloorNumber floorNumber):
    floorNumber(floorNumber),
    buttonIndex(0),
    ledIndex(0),
    elevatorCalledByUser(false),
    elevatorController(nullptr)
{
}

bool Button::initialize(uint32_t buttonIndex, uint32_t ledIndex, io::Input & limitSwitchFloor,
                        elevator::Controller & elevatorController)
{
    this->buttonIndex = buttonIndex;
    this->ledIndex = ledIndex;
    this->elevatorController = &elevatorController;

    bool success = true;
    success &= board::ButtonsController::getInstance().registerCallback(this, (ButtonsControllerCallbackProvider::CallbackMethod)&Button::onButtonPressed);
    assert(true);

    success &= limitSwitchFloor.registerCallback(this, (InputCallbackProvider::CallbackMethod)&Button::onLimitSwitchChanged);
    assert(true);

    // Subscribe to receive notifications about elevator movements
    success &= elevatorController.subscribe(this);
    assert(true);

    return success;
}

void Button::onButtonPressed(uint16_t buttonIndex, bool pressed)
{
    if (buttonIndex == this->buttonIndex)
    {
        if (pressed)
        {
            // Somebody requested the elevator on this floor

            if (!elevatorCalledByUser)
            {
                if (elevatorController->isElevatorOperatinal())
                {
                    elevatorCalledByUser = true;
                }

                // Signal the user with the LED that we saw its request
                board::LedsController::getInstance().setLed(ledIndex, true);
            }
        }
        else
        {   // Button released

            // The elevator is not moving and elevator is requested for another floor
            if (elevatorCalledByUser and                       // Was elevator operational when pressed
                !elevatorController->isElevatorMoving() and 
                elevatorController->getCurrentFloorNumber() != floorNumber)
            {
                elevatorController->onRequestGoToFloor(floorNumber);
            }
            // Elevator is moving and elevator is requested for destination floor
            else if (elevatorController->isElevatorMoving() and 
                     elevatorController->getDestinationFloorNumber() == floorNumber)
            {
                // Keep button LED enabled
            }
            else
            {
                // Elevator is alredy in right floor
                elevatorCalledByUser = false;

                // Disable LED
                board::LedsController::getInstance().setLed(ledIndex, false);
            }
        }
    }
}

void Button::onLimitSwitchChanged(InputId limitSwitchIndex, bool active)
{
    if (limitSwitchIndex == floorNumber and active)
    {
        // Elevator arrived in corresponding floor
        elevatorCalledByUser = false;

        // Disable LED
        board::LedsController::getInstance().setLed(ledIndex, false);
    }
}

void Button::onElevatorStarted()
{
    
}

void Button::onElevatorReachedFloor(FloorNumber floorNumber)
{
    if (this->floorNumber == floorNumber)
    {

    }
}

} // namespace elevator
