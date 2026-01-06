#ifndef MOTOR_DECODER_H
#define MOTOR_DECODER_H

#include "xf/behavior.h"
#include "driver.h"

namespace motor {

/**
 * @brief Motor position decoder using the motor position sensor (encoder).
 */
class Decoder : protected XFBehavior
{
public:
    using Direction = Driver::Direction;

    Decoder();

    bool initialize(const Driver & driver);
    inline void start() { startBehavior(); }

    int32_t getPosition() const;            ///< Returns the position of the motor (degrees on the motors spindle).
    void resetPosition();                   ///< Sets the position information to zero.
    void setPosition(int32_t newPosition);  ///< Forces the position to a specific value (for recalibration).

    Driver::Direction getDirection() const; ///< Returns the measured direction of the motor. Retuns DIRECTION_UNKNOWN when motor is not moving.

    // State machine implementation
protected:
    XFEventStatus processEvent() override;

protected:
    bool executeOnce();
    int32_t calculateAngleDifference();     ///< Function to calculate the angle difference considering the overflow.

protected:
    const Driver * motorDriver;

    int32_t position;                       ///< Holds the motors position.
    int32_t currentAngle, previousAngle;    ///< Attributes to calculate the angle difference.

    int32_t direction;                      ///< Direction information received from the Zephyr driver.
};

} // namespace motor
#endif // MOTOR_DECODER_H
