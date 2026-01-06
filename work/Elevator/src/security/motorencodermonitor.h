#ifndef SECURITY_MOTOR_ENCODER_MONITOR_H
#define SECURITY_MOTOR_ENCODER_MONITOR_H

#include <cstdint>
#include <cstdlib>           // abs
#include <zephyr/kernel.h>

#include "monitor.h"
#include "controller.h"
#include "motor/decoder.h"
#include "interface/elevatorcontrollerobserver.h"

namespace security {

class MotorEncoderMonitor : public Monitor, public interface::ElevatorControllerObserver
{
public:
    MotorEncoderMonitor();
    ~MotorEncoderMonitor();

    void initialize(elevator::Controller & controller, motor::Decoder & decoder);
    void start();
    void stop();

protected:
    // ElevatorControllerObserver
    void onElevatorStarted() override;
    void onElevatorReachedFloor(FloorNumber floorNumber) override;
    void onElevatorError() override;

private:
    // State machine states
    typedef enum {
        ST_INIT,
        ST_WAIT_4_ELEVATOR_2_START,
        ST_MONITORING_MOTOR_POSITION,
        ST_MOTOR_ERROR
    } SMStates;

    // State machine events
    typedef enum {
        evElevatorStarted,
        evElevatorReachedFloor,
        evCheckTimeout,
        evError
    } SMEvents;

    void SM_processEvent(SMEvents eventId);

    bool checkMotorPositionOk();

    // Timer
    static void timerCallback(struct k_timer * timer);
    void handleTimerTimeout();

private:
    elevator::Controller * controller_ {nullptr};
    motor::Decoder * decoder_ {nullptr};

    SMStates currentState {ST_WAIT_4_ELEVATOR_2_START};

    // Encoder monitoring data
    int32_t lastPos {0};
    int32_t lastMoveTime {0};   // time in ms when we last detected movement

    // Avoid spamming security issue
    bool errorAlreadyNotified {false};

    // Timer for periodic checks
    struct k_timer checkTimer;
    const uint32_t CHECK_INTERVAL_MS = 200;
    bool timerActive {false};

    // Simple parameters (tu peux ajuster)
    const int32_t MIN_MOVEMENT_DEG = 2;
    const int32_t STALL_TIMEOUT_MS = 800;
};

} // namespace security

#endif // SECURITY_MOTOR_ENCODER_MONITOR_H
