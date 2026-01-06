#include "motorencodermonitor.h"
#include "trace/trace.h"
#include <zephyr/kernel.h>
#include <cstdlib>   // abs

namespace security {

// Tunables (mets-les dans le .h si tu préfères)
static constexpr int32_t CHECK_INTERVAL_MS = 200;

// Mouvement minimum pour considérer que “ça bouge vraiment”
// (évite de compter le bruit / jitter du capteur)
static constexpr int32_t MIN_MOVEMENT_DEG = 2;

// Temps max sans mouvement significatif avant erreur
static constexpr int32_t STALL_TIMEOUT_MS = 800; // 0.8s (ajuste selon la vitesse de ta maquette)

MotorEncoderMonitor::MotorEncoderMonitor()
    : Monitor(ISSUE_MOTOR_NO_MORE_SPINNING) 
{
    k_timer_init(&checkTimer, &MotorEncoderMonitor::timerCallback, nullptr);
}

MotorEncoderMonitor::~MotorEncoderMonitor()
{
    stop();
}

void MotorEncoderMonitor::initialize(elevator::Controller & controller, motor::Decoder & decoder)
{
    controller_ = &controller;
    decoder_ = &decoder;

    timerActive = false;
    errorAlreadyNotified = false;

    currentState = ST_WAIT_4_ELEVATOR_2_START;

    lastPos = 0;
    lastMoveTime = 0;

    if (controller_)
    {
        bool subscribeOk = controller_->subscribe(this);
        Trace::out("MotorEncoderMonitor: Subscribed to controller - result: %d", subscribeOk);
    }
}

void MotorEncoderMonitor::start()
{
    if (!timerActive)
    {
        timerActive = true;
        errorAlreadyNotified = false;
        currentState = ST_WAIT_4_ELEVATOR_2_START;
        Trace::out("MotorEncoderMonitor: Started");
    }
}

void MotorEncoderMonitor::stop()
{
    if (timerActive)
    {
        timerActive = false;
        k_timer_stop(&checkTimer);
        Trace::out("MotorEncoderMonitor: Stopped");
    }
}

void MotorEncoderMonitor::onElevatorStarted()
{
    Trace::out("MotorEncoderMonitor: Elevator started");
    SM_processEvent(evElevatorStarted);

    if (!decoder_)
        return;

    // Stocker la position initiale + timestamp
    lastPos = decoder_->getPosition();
    lastMoveTime = (int32_t)k_uptime_get();

    // Lancer timer de surveillance
    if (currentState == ST_MONITORING_MOTOR_POSITION && timerActive && !errorAlreadyNotified)
    {
        k_timer_start(&checkTimer, K_MSEC(CHECK_INTERVAL_MS), K_MSEC(CHECK_INTERVAL_MS));
    }
}

void MotorEncoderMonitor::onElevatorReachedFloor(FloorNumber floorNumber)
{
    Trace::out("MotorEncoderMonitor: Elevator reached floor %d", floorNumber);
    SM_processEvent(evElevatorReachedFloor);
    k_timer_stop(&checkTimer);
}

void MotorEncoderMonitor::onElevatorError()
{
    Trace::out("MotorEncoderMonitor: Elevator error detected");
    k_timer_stop(&checkTimer);

    errorAlreadyNotified = true;
    
    if (currentState != ST_MOTOR_ERROR)
    {
        SM_processEvent(evError);
    }
}

bool MotorEncoderMonitor::checkMotorPositionOk()
{
    if (!decoder_)
    {
        return false;
    }

    int32_t pos = decoder_->getPosition();
    int32_t now = (int32_t)k_uptime_get();

    // If motors move, it's OK
    if (std::abs(pos - lastPos) >= MIN_MOVEMENT_DEG)
    {
        lastPos = pos;
        lastMoveTime = now;
        return true;
    }

    // We verify if it didn't move for more than STALL TIMEOUT
    if ((now - lastMoveTime) >= STALL_TIMEOUT_MS)
    {
        Trace::out("MotorEncoderMonitor: Motor seems stuck (pos=%ld)", pos);
        return false;
    }

    return true;
}


void MotorEncoderMonitor::SM_processEvent(SMEvents eventId)
{
    const SMStates oldState = currentState;

    switch (currentState)
    {
        case ST_WAIT_4_ELEVATOR_2_START:
        {
            if (eventId == evElevatorStarted)
            {
                currentState = ST_MONITORING_MOTOR_POSITION;
            }
            break;
        }

        case ST_MONITORING_MOTOR_POSITION:
        {
            if (eventId == evElevatorReachedFloor)
            {
                currentState = ST_WAIT_4_ELEVATOR_2_START;
            }
            else if (eventId == evCheckTimeout)
            {
                if (!checkMotorPositionOk())
                {
                    currentState = ST_MOTOR_ERROR;
                }
            }
            else if (eventId == evError)
            {
                currentState = ST_MOTOR_ERROR;
            }
            break;
        }

        case ST_MOTOR_ERROR:
        default:
            break;
    }

    if (oldState != currentState)
    {
        switch (currentState)
        {
            case ST_WAIT_4_ELEVATOR_2_START:
                Trace::out("MotorEncoderMonitor: Waiting for elevator to start");
                break;

            case ST_MONITORING_MOTOR_POSITION:
                Trace::out("MotorEncoderMonitor: Monitoring motor position");
                break;

            case ST_MOTOR_ERROR:
                Trace::out("MotorEncoderMonitor: MOTOR ERROR - encoder stalled!");
                if (!errorAlreadyNotified)
                {
                    errorAlreadyNotified = true;
                    notifySecurityIssue(ISSUE_MOTOR_NO_MORE_SPINNING);
                }
                break;

            default:
                break;
        }
    }
}

// Timer callback
void MotorEncoderMonitor::timerCallback(struct k_timer * timer)
{
    MotorEncoderMonitor * pThis = CONTAINER_OF(timer, MotorEncoderMonitor, checkTimer);
    pThis->handleTimerTimeout();
}

void MotorEncoderMonitor::handleTimerTimeout()
{
    if (currentState == ST_MONITORING_MOTOR_POSITION && !errorAlreadyNotified)
    {
        SM_processEvent(evCheckTimeout);
    }
}

} // namespace security
