#ifndef ELEVATOR_BUTTON_H
#define ELEVATOR_BUTTON_H

#include <cstdint>
#include "interface/buttonscontrollercallbackprovider.h"
#include "interface/inputcallbackprovider.h"
#include "interface/elevatorcontrollerobserver.h"
#include "io/input.h"

namespace elevator {

class Controller;

/**
 * @brief The elevator::Button handles a button on a floor used to call the elevator.
 * 
 * It is also used for a button in the elevator itself to select the floor to move to.
 * 
 * The elevator::Button also handles the corresponding LED assiciated to the button.
 * 
 * The class memorizes the call made by the user and shows it using the associated LED.
 */
class Button : protected interface::ButtonsControllerCallbackProvider,
               protected interface::InputCallbackProvider,
               protected interface::ElevatorControllerObserver
{
public:
    using InputId = io::Input::InputId;

    Button(FloorNumber floorNumber);
    bool initialize(uint32_t buttonIndex, uint32_t ledIndex, io::Input & limitSwitchFloor, 
                    elevator::Controller & elevatorController);

    // Interface ButtonsControllerCallbackProvider implementation
protected:
    void onButtonPressed(uint16_t buttonIndex, bool pressed);

    // Callback method for limit switchs
protected:
    void onLimitSwitchChanged(InputId limitSwitchIndex, bool active);

    // Callback method for ElevatorControllerObserver interface
protected:
    void onElevatorStarted() override;
    void onElevatorReachedFloor(FloorNumber floorNumber) override;
    void onElevatorError() override {}

protected:
    const FloorNumber floorNumber;  ///< Floor number to which the floor button controller is associated to.
    uint32_t buttonIndex;           ///< Index to corresponding button.
    uint32_t ledIndex;              ///< Index of LED driven by the button controller.
    bool elevatorCalledByUser;      ///< Gets true when user requests elevator.

    elevator::Controller * elevatorController;
};

} // namespace elevator
#endif // ELEVATOR_BUTTON_H
