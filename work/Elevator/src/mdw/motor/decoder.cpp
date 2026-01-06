#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include "trace/trace.h"
#include "decoder.h"

namespace motor {

Decoder::Decoder():
    XFBehavior(/* active = */ true),
    motorDriver(nullptr),
    position(0),
    currentAngle(0),
    previousAngle(0),
    direction(-1)
{
}

bool Decoder::initialize(const Driver & driver)
{
    const struct device * const dev = DEVICE_DT_GET(DT_ALIAS(qdec0));

    motorDriver = &driver;

	if (!device_is_ready(dev))
    {
		Trace::out("Decoder device is not ready");
		return false;
	}

    return true;
}

int32_t Decoder::getPosition() const
{
    return position;
}

void Decoder::resetPosition()
{
    position = 0;
}

void Decoder::setPosition(int32_t newPosition)
{
    position = newPosition;
}

Driver::Direction Decoder::getDirection() const
{
    if (motorDriver and
        motorDriver->isRunning() and
        (direction == 0 or direction == 16))
    {
        return (direction == 0) ? Driver::FORWARD : Driver::BACKWARD;
    }
    return Driver::DIRECTION_UNKNOWN;
}

bool Decoder::executeOnce()
{
    const struct device * const dev = DEVICE_DT_GET(DT_ALIAS(qdec0));

    if (device_is_ready(dev))
    {
        struct sensor_value val;
        int rc;

        rc = sensor_sample_fetch(dev);
        if (rc != 0) 
        {
            Trace::out("Failed to fetch sample (%d)", rc);
            return false;
        }

        // Get position
        rc = sensor_channel_get(dev, SENSOR_CHAN_ROTATION, &val);
        if (rc != 0)
        {
            Trace::out("Failed to get position data (%d)", rc);
            return false;
        }
        const int32_t angle = val.val1;

        // Get direction
        rc = sensor_channel_get(dev, SENSOR_CHAN_CURRENT, &val);
        if (rc != 0)
        {
            Trace::out("Failed to get direction data (%d)", rc);
            return false;
        }
        direction = val.val1;

        //Trace::out("Decoder: Position = %d degrees, direction: %d", angle, direction);

        // Update absolute position
        currentAngle = angle;
        position += calculateAngleDifference();

        //Trace::out("Decoder: Absolute Position: %ld", position);
    }

    return true;
}

int32_t Decoder::calculateAngleDifference()
{
    // Ensure current angle is within [0, 360] degrees
    currentAngle %= 360;

    // Calculate the difference
    int32_t difference = currentAngle - previousAngle;

    if (difference != 0)
    {
        // Adjust the difference for the overflow case
        if (difference > 180)       // Check underflow
        {
            difference -= 360;
        } 
        else if (difference < -180) // Check overflow
        {
            difference += 360;
        }

        previousAngle = currentAngle;
    }

    return difference;
}

XFEventStatus Decoder::processEvent()
{
    executeOnce();
    pushEvent(100, 100);

    return XFEventStatus::Consumed;
}

} // namespace motor
