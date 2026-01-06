#ifndef SECURITY_MOTOR_ENCODER_MONITOR_H
#define SECURITY_MOTOR_ENCODER_MONITOR_H

#include <cstdint>
#include <zephyr/kernel.h>
#include "monitor.h"
#include "controller.h"
#include "motor/decoder.h"
#include "interface/elevatorcontrollerobserver.h"

namespace security{
    class MotorEncoderMonitor : public Monitor, public interface::ElevatorControllerObserver{
        public:
            MotorEncoderMonitor();
            ~MotorEncoderMonitor();
            
            void initialize(elevator::Controller & controller, motor::Decoder & decoder);
            void start();
            void stop();

        protected:
            // ElevatorControllerObserver interface implementation
            void onElevatorStarted() override;
            void onElevatorReachedFloor(FloorNumber floorNumber) override;
            void onElevatorError() override;

        private:
            // State types
            typedef enum {
                ST_INIT,
                ST_WAIT_4_ELEVATOR_2_START,
                ST_MONITORING_ELEVATOR,
                ST_LIFT_ERROR
            } SMStates;

            typedef enum {
                evElevatorStarted,
                evElevatorReachedFloor,
                evCheckPositionTimeout,
                evError
            } SMEvents;

            // State machine processing
            void SM_processEvent(SMEvents eventId);

            // Timer callback (static)
            static void timerCallback(struct k_timer * timer);
            void handleTimerTimeout();

            // Members
            elevator::Controller * controller_ {nullptr};
            motor::Decoder * decoder_ {nullptr};
            SMStates currentState {ST_WAIT_4_ELEVATOR_2_START};
    };
}
#endif