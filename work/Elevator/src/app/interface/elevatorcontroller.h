#ifndef INTERFACE_ELEVATOR_CONTROLLER_SUBJECT_H
#define INTERFACE_ELEVATOR_CONTROLLER_SUBJECT_H

#include <cstdint>

namespace interface {

class ElevatorControllerObserver;

/**
 * @brief Interface used by the classes observing the elevator::Controller.
 */
class ElevatorController
{
public:
    using FloorNumber = uint32_t;   ///< Starting from 0 (floor 0). Note: Internally, the first floor is 'floor 0'! 

    virtual ~ElevatorController() = default;

public:
    virtual bool subscribe(ElevatorControllerObserver * observer) = 0;          ///< Registers a new observer. Returns true on success.
    virtual void unsubscribe(ElevatorControllerObserver * observer) = 0;        ///< Un-subscribes a registered observer.

protected:
    virtual void notifyElevatorStarted() = 0;                                   ///< Notifies observers that elevator started to move to another floor.
    virtual void notifyElevatorReachedFloor(FloorNumber floorNumber) = 0;       ///< Notifies observers about elevator reached destination floor.
    virtual void notifyElevatorError() = 0;                                     ///< Notifies observers that elevator has an error.

protected:
    ElevatorController() = default;                                             ///< Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_ELEVATOR_CONTROLLER_SUBJECT_H
