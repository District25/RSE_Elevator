#ifndef SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H
#define SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H

#include <cstdint>
#include <zephyr/kernel.h>

#include "monitor.h"
#include "controller.h"
#include "security-config.h"

#include "interface/inputcallbackprovider.h"
#include "interface/elevatorcontrollerobserver.h"
#include "io/input.h"

namespace security {

class FloorLimitSwitchesMonitor : protected interface::InputCallbackProvider,
                                 public Monitor,
                                 public interface::ElevatorControllerObserver
{
public:
    using InputId = io::Input::InputId;

    FloorLimitSwitchesMonitor();
    ~FloorLimitSwitchesMonitor();

    // 3 capteurs d'étages (adapter si tu as plus/moins)
    void initialize(elevator::Controller & controller,
                    io::Input & limitSwitch0,
                    io::Input & limitSwitch1);

    void start();
    void stop();

protected:
    // ElevatorControllerObserver
    void onElevatorStarted() override;
    void onElevatorReachedFloor(FloorNumber floorNumber) override;
    void onElevatorError() override;

protected:
    // InputCallbackProvider
    void onLimitSwitchChanged(InputId limitSwitchIndex, bool active);

private:
    // State machine
    typedef enum {
        ST_WAIT_4_ELEVATOR_2_START,
        ST_MONITORING_SWITCHES,
        ST_SWITCH_ERROR
    } SMStates;

    typedef enum {
        evElevatorStarted,
        evElevatorReachedFloor,
        evCheckTimeout,
        evError
    } SMEvents;

    void SM_processEvent(SMEvents eventId);

    // Checks
    bool checkDuringMoveOk();
    bool checkAtStopOk(FloorNumber floorNumber);
    uint32_t countActive() const;

    // Timer
    static void timerCallback(struct k_timer * timer);
    void handleTimerTimeout();

private:
    elevator::Controller * controller_ {nullptr};

    // état des switches: index 0..2
    bool swActive[2] {false, false};

    SMStates currentState {ST_WAIT_4_ELEVATOR_2_START};

    struct k_timer checkTimer;
    const uint32_t CHECK_INTERVAL_MS = 200;

    bool timerActive {false};
    bool errorAlreadyNotified {false};

    // petite tolérance juste après démarrage (le switch peut rester actif un tout petit moment)
    int32_t startTsMs {0};
    const int32_t START_GRACE_MS = 300;
};

} // namespace security

#endif // SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H
