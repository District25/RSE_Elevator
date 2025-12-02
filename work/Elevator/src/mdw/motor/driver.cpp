#include <cassert>
#include "driver.h"

namespace motor {

Driver * Driver::instance(nullptr);

Driver::Driver():
    motorEnabled(false),
    lastDirection(DIRECTION_UNKNOWN),
    gpioMotorEnable GPIO_DT_SPEC_GET(DT_ALIAS(motor_enable), gpios),
    gpioMotorDirection GPIO_DT_SPEC_GET(DT_ALIAS(motor_direction), gpios)
{
    assert(!instance);      // Singleton pattern. Only one instance allowed
    instance = this;        // Store 'this' to static pointer

    gpio_pin_configure_dt(&gpioMotorEnable, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&gpioMotorDirection, GPIO_OUTPUT_INACTIVE);
}

bool Driver::initialize()
{
    return true;
}

void Driver::enableMotor(Direction direction /* = FORWARD */)
{
    setDirection(direction);

    motorEnabled = true;
    gpio_pin_set(gpioMotorEnable.port, gpioMotorEnable.pin, 1);    
}

void Driver::disableMotor()
{
    motorEnabled = false;
    gpio_pin_set(gpioMotorEnable.port, gpioMotorEnable.pin, 0);
}

bool Driver::isRunning() const
{
    //return (bool)gpio_pin_get(gpioMotorEnable.port, gpioMotorEnable.pin);   
    return motorEnabled;
}

Driver::Direction Driver::getDirection() const
{
    return lastDirection;
}

void Driver::setDirection(Direction direction)
{
    assert(direction != DIRECTION_UNKNOWN);
    lastDirection = direction;
    gpio_pin_set(gpioMotorDirection.port, gpioMotorDirection.pin, (direction == BACKWARD) ? 1 : 0);
}

} // namespace motor