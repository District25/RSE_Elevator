#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <cassert>
#include <zephyr/drivers/gpio.h>

namespace motor {

/**
 * @brief Class to drive the motor.
 * 
 * Assumes that only one motor is present: Singleton Pattern.
 */
class Driver
{
public:
    typedef enum { DIRECTION_UNKNOWN = 0, BACKWARD = 1, FORWARD = 2 } Direction;

    Driver();

    bool initialize();

    static Driver & getInstance() { assert(instance); return *instance; }   ///< Access to single instance.

    void enableMotor(Direction direction = FORWARD);
    void disableMotor();

    bool isRunning() const;

    Direction getDirection() const;

protected:
    void setDirection(Direction direction);

protected:
    static Driver * instance;

    bool motorEnabled;                          ///< True if motor is enabled.
    Direction lastDirection;                    ///< Last direction set.

    struct gpio_dt_spec gpioMotorEnable;
    struct gpio_dt_spec gpioMotorDirection;
};

} // namespace motor
#endif // MOTOR_DRIVER_H