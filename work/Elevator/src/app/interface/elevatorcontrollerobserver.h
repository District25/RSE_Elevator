#ifndef INTERFACE_ELEVATOR_CONTROLLER_OBSERVER_H
#define INTERFACE_ELEVATOR_CONTROLLER_OBSERVER_H

#include "elevatorcontroller.h"

namespace elevator { class Controller; }

namespace interface {

/**
 * @brief Interface to be provided by the class who wants to receive notifications from the elevator::Controller.
 */
class ElevatorControllerObserver
{
	friend class ::elevator::Controller;

public:
	using FloorNumber = ElevatorController::FloorNumber;

    virtual ~ElevatorControllerObserver() = default;

protected:
	virtual void onElevatorStarted() = 0;								///< Called when the elevator starts.
	virtual void onElevatorReachedFloor(FloorNumber floorNumber) = 0;	///< Called by the subject upon a button short pressed.
	virtual void onElevatorError() = 0;									///< Called when elevator passes into an error.

protected:
    ElevatorControllerObserver() = default;                				///< Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_ELEVATOR_CONTROLLER_OBSERVER_H
