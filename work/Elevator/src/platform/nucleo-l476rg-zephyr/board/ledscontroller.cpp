#include "mcu/mcu.h"
#include "config/ledscontroller-config.h"
#if (LEDSCONTROLLER_TRACE_ENABLE != 0)
    #include "trace/trace.h"
#endif // LEDSCONTROLLER_TRACE_ENABLE
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "ledscontroller.h"

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led_in_floor_1), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led_in_floor_2), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led_call_elevator_floor_1), gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led_call_elevator_floor_2), gpios);
static const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(DT_ALIAS(led_elevator_move_up), gpios);
static const struct gpio_dt_spec led5 = GPIO_DT_SPEC_GET(DT_ALIAS(led_elevator_move_down), gpios);
static const struct gpio_dt_spec led6 = GPIO_DT_SPEC_GET(DT_ALIAS(led_elevator_error), gpios);
static const struct gpio_dt_spec led7 = GPIO_DT_SPEC_GET(DT_ALIAS(led_elevator_operational), gpios);

namespace board {

LedsController * LedsController::instance = nullptr;

LedsController::LedsController()
{
    assert(!instance);	// Only one instance of this class allowed!
    instance = this;

	// Initialize the method array with the right methods.
	ledOperation_[0] = &LedsController::setLed0;
	ledOperation_[1] = &LedsController::setLed1;
	ledOperation_[2] = &LedsController::setLed2;
	ledOperation_[3] = &LedsController::setLed3;
	ledOperation_[4] = &LedsController::setLed4;
	ledOperation_[5] = &LedsController::setLed5;
	ledOperation_[6] = &LedsController::setLed6;
	ledOperation_[7] = &LedsController::setLed7;

	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led4, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led5, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led6, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led7, GPIO_OUTPUT_INACTIVE);
}

LedsController::~LedsController()
{
}

bool LedsController::initialize()
{
	// Arrange LEDs so it makes a sequence from buttom up
	const uint32_t patcher[] {0, 2, 5, 4, 3, 1, 6, 7};

    // Test LEDs
    for (uint32_t i = 0; i < LED_COUNT; i++)
    {
        setLed(patcher[i], true);
        k_sleep(K_MSEC(200));
        setLed(patcher[i], false);
    }

    return true;
}

void LedsController::setLed(uint8_t index, bool bOn)
{
	setLeds(0x01 << index, bOn);
}

void LedsController::setLeds(uint8_t ledMask, bool bOn)
{
	uint8_t mask = 0x01;

	for (uint8_t i = 0; i < ledCount(); i++, mask <<= 1)
	{
		if ((ledMask & mask) == mask && ledOperation_[i])
		{
			(this->*ledOperation_[i])(bOn);
		}
	}
}

void LedsController::setLed0(bool bOn /* = true */)
{
	if (bOn)
	{
		gpio_pin_set(led0.port, led0.pin, 1);
	}
	else
	{
		gpio_pin_set(led0.port, led0.pin, 0);
	}

#if (LEDSCONTROLLER_TRACE_ENABLE != 0)
	if (bOn)
	{
		// Not using "%s" here (bug in gcc c-library!)
		Trace::out(" LED0: on");
	}
	else
	{
		Trace::out(" LED0: off");
	}
#endif // LEDSCONTROLLER_TRACE_ENABLE
}

void LedsController::setLed1(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led1.port, led1.pin, 1);
    }
    else
    {
		gpio_pin_set(led1.port, led1.pin, 0);
    }

#if (LEDSCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("  LED1: on");
	}
	else
	{
		Trace::out("  LED1: off");
	}
#endif // LEDSCONTROLLER_TRACE_ENABLE
}

void LedsController::setLed2(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led2.port, led2.pin, 1);
    }
    else
    {
		gpio_pin_set(led2.port, led2.pin, 0);
    }

#if (LEDSCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("   LED2: on");
	}
	else
	{
		Trace::out("   LED2: off");
	}
#endif // LEDSCONTROLLER_TRACE_ENABLE
}

void LedsController::setLed3(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led3.port, led3.pin, 1);
    }
    else
    {
		gpio_pin_set(led3.port, led3.pin, 0);
    }

#if (LEDSCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("    LED3: on");
	}
	else
	{
		Trace::out("    LED3: off");
	}
#endif // LEDSCONTROLLER_TRACE_ENABLE
}

void LedsController::setLed4(bool bOn /* = true */)
{
    gpio_pin_set(led4.port, led4.pin, (bOn) ? 1 : 0);
}

void LedsController::setLed5(bool bOn /* = true */)
{
    gpio_pin_set(led5.port, led5.pin, (bOn) ? 1 : 0);
}

void LedsController::setLed6(bool bOn /* = true */)
{
    gpio_pin_set(led6.port, led6.pin, (bOn) ? 1 : 0);
}

void LedsController::setLed7(bool bOn /* = true */)
{
    gpio_pin_set(led7.port, led7.pin, (bOn) ? 1 : 0);
}

} // namespace board