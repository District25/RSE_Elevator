#ifndef SECURITY_MAXIMUM_LIFT_MONITOR_H
#define SECURITY_MAXIMUM_LIFT_MONITOR_H

#include "controller.h"
#include "motor/decoder.h"
#include "interface/elevatorcontrollerobserver.h"
#pragma once

namespace security {

    class MaximumLiftMonitor
    {
        public:
            MaximumLiftMonitor();
            void initialize(elevator::Controller & controller, motor::Decoder & decoder);
            /*elevator::Controller * get(){return controller_;}
            void set(elevator::Controller* toto){controller_ = toto;}*/

        protected:
            void onElevatorStarted() override;
            void onElevatorReachedFloor(FloorNumber floorNumber) override;
            void onElevatorError() override {}
        private:
            elevator::Controller * controller_ {nullptr};
            motor::Decoder * decoder_ {nullptr};

        protected:
            const FloorNumber floorNumber;
    };
} // end of namespace

#endif