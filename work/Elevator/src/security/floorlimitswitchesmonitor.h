#ifndef SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H
#define SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H

#include <cstdint>
#include <zephyr/kernel.h>

#include "monitor.h"
#include "controller.h"
#include "../config/security-config.h"

#include "interface/elevatorcontrollerobserver.h"
#include "io/input.h"

namespace security {

class FloorLimitSwitchesMonitor : public Monitor,
                                 public interface::ElevatorControllerObserver
{
public:
    FloorLimitSwitchesMonitor();
    ~FloorLimitSwitchesMonitor();

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

    // Polling
    void updateSwitchStates();

    // Timer
    static void timerCallback(struct k_timer * timer);
    void handleTimerTimeout();

private:
    elevator::Controller * controller_ {nullptr};

    // Pointers to physical inputs (polling)
    io::Input * swInput[2] {nullptr, nullptr};

    // cached states
    bool swActive[2] {false, false};

    // issue to send
    IssueNumber pendingIssue {UNKNOWN_ISSUE};

    SMStates currentState {ST_WAIT_4_ELEVATOR_2_START};

    struct k_timer checkTimer;
    const uint32_t CHECK_INTERVAL_MS = 200;

    bool timerActive {false};
    bool errorAlreadyNotified {false};

    // grace period after start
    int32_t startTsMs {0};
    const int32_t START_GRACE_MS = 300;
};

} // namespace security

#endif // SECURITY_FLOOR_LIMIT_SWITCHES_MONITOR_H
