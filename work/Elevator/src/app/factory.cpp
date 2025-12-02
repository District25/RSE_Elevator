#include "trace/trace.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "mdw/io/input.h"
#include "board/hal/buttons.h"
#include "board/buttonscontroller.h"
#include "board/ledscontroller.h"
#include "mdw/motor/driver.h"
#include "mdw/motor/decoder.h"
#include "security/factory.h"
#include "controller.h"
#include "button.h"
#include "factory.h"

namespace app
{

// static
void Factory::initialize()
{
    Trace::initialize();
    
    board::hal::buttons::initialize();

    security::Factory::preInitialize();

    Trace::out("Factory: Initializing app components...");

    limitSwitchFloor1().initialize(GPIO_DT_SPEC_GET(DT_ALIAS(limit_switch_floor_1), gpios), 0);
    limitSwitchFloor2().initialize(GPIO_DT_SPEC_GET(DT_ALIAS(limit_switch_floor_2), gpios), 1);
    buttonsController().initialize();
    ledsController().initialize();
    motorDriver().initialize();
    motorDecoder().initialize(motorDriver());
    controller().initialize(limitSwitchFloor1(), limitSwitchFloor2());
    buttonFloor1().initialize(/*buttonIndex = */ 0, /* ledIndex = */ 2, limitSwitchFloor1(), controller());
    buttonFloor2().initialize(/*buttonIndex = */ 1, /* ledIndex = */ 3, limitSwitchFloor2(), controller());
    buttonElevatorDown().initialize(/*buttonIndex = */ 3, /* ledIndex = */ 5, limitSwitchFloor1(), controller());
    buttonElevatorUp().initialize(/*buttonIndex = */ 2, /* ledIndex = */ 4, limitSwitchFloor2(), controller());
    
    security::Factory::initialize();
}

// static
void Factory::build()
{
    security::Factory::build();

    Trace::out("Factory: Starting app components...");

    // Start state machine(s)
    buttonsController().start();
    motorDecoder().start();
    controller().start();

    Trace::out("Factory: App ready");
}

io::Input & Factory::limitSwitchFloor1()
{
    static io::Input limitSwitchFloor1;
    return limitSwitchFloor1;
}

io::Input & Factory::limitSwitchFloor2()
{
    static io::Input limitSwitchFloor2;
    return limitSwitchFloor2;
}

board::ButtonsController & Factory::buttonsController()
{
    static board::ButtonsController buttonsController;
    return buttonsController;
}

board::LedsController & Factory::ledsController()
{
    static board::LedsController ledsController;
    return ledsController;
}

motor::Driver & Factory::motorDriver()
{
    static motor::Driver driver;
    return driver;
}

motor::Decoder &Factory::motorDecoder()
{
    static motor::Decoder decoder;
    return decoder;
}

elevator::Controller & Factory::controller()
{
    static elevator::Controller controller;
    return controller;
}

elevator::Button & Factory::buttonFloor1()
{
    static elevator::Button buttonFloor(0);
    return buttonFloor;
}

elevator::Button & Factory::buttonFloor2()
{
    static elevator::Button buttonFloor(1);
    return buttonFloor;
}

elevator::Button &Factory::buttonElevatorDown()
{
    static elevator::Button buttonElevator(0);
    return buttonElevator;
}

elevator::Button &Factory::buttonElevatorUp()
{
    static elevator::Button buttonElevator(1);
    return buttonElevator;
}

} // namespace app

void Factory_initialize()
{
    app::Factory::initialize();
}

void Factory_build()
{
    app::Factory::build();
}
