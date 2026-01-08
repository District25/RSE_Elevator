#include "trace/trace.h"
#include "app/factory.h"
#include "factory.h"

namespace security {

void Factory::preInitialize()
{
    // Note: The security::IssueNotifier needs to be created and initialised in
    //       an early stage. It may get called by observers trying to subscribe
    //       to security issues.
    getIssueNotifier().initialize();
}

// static
void Factory::initialize()
{
    Trace::out("Factory: Initializing security components...");

    // Initialize MaximumLiftMonitor
    getMaximumLiftMonitor().initialize(app::Factory::controller(), app::Factory::motorDecoder());

    // Initialize MotorEncoderMonitor
    getMotorEncoderMonitor().initialize(app::Factory::controller(), app::Factory::motorDecoder());

    // Initialize FloorLimitSwitchesMonitor (Task3)
    getFloorLimitSwitchesMonitor().initialize(
        app::Factory::controller(),
        app::Factory::limitSwitchFloor1(),
        app::Factory::limitSwitchFloor2()
    );
}

// static
void Factory::build()
{
    Trace::out("Factory: Starting security components...");

    // Start MaximumLiftMonitor
    getMaximumLiftMonitor().start();

    // Start MotorEncoderMonitor
    getMotorEncoderMonitor().start();

    // Start FloorLimitSwitchesMonitor
    getFloorLimitSwitchesMonitor().start();
}

// static
IssueNotifier & Factory::getIssueNotifier()
{
    static IssueNotifier issueNotifier;
    return issueNotifier;
}

// static
MaximumLiftMonitor & Factory::getMaximumLiftMonitor()
{
    static MaximumLiftMonitor maximumLiftMonitor;
    return maximumLiftMonitor;
}

// static
MotorEncoderMonitor & Factory::getMotorEncoderMonitor()
{
    static MotorEncoderMonitor motorEncoderMonitor;
    return motorEncoderMonitor;
}

// static
FloorLimitSwitchesMonitor & Factory::getFloorLimitSwitchesMonitor()
{
    static FloorLimitSwitchesMonitor floorLimitSwitchesMonitor;
    return floorLimitSwitchesMonitor;
}

} // namespace security
