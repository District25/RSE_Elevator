#include "maximumliftmonitor.h"

namespace security{

    void MaximumLiftMonitor::initialize(elevator::Controller & controller, motor::Decoder & decoder){
        controller_ = &controller;
        decoder_ = &decoder;
    }   
} // end of namespace