#include "floorlimitswitchesmonitor.h"
#include "trace/trace.h"

namespace security {

FloorLimitSwitchesMonitor::FloorLimitSwitchesMonitor()
    : Monitor(ISSUE_LIMIT_SWITCHES)   // recommande: ajouter une issue dédiée
{
    k_timer_init(&checkTimer, &FloorLimitSwitchesMonitor::timerCallback, nullptr);
}

FloorLimitSwitchesMonitor::~FloorLimitSwitchesMonitor()
{
    stop();
}

void FloorLimitSwitchesMonitor::initialize(elevator::Controller & controller,
                                           io::Input & limitSwitch0,
                                           io::Input & limitSwitch1)
{
    controller_ = &controller;

    swActive[0] = false;
    swActive[1] = false;

    timerActive = false;
    errorAlreadyNotified = false;
    currentState = ST_WAIT_4_ELEVATOR_2_START;

    // Subscribe controller events
    if (controller_)
    {
        bool subscribeOk = controller_->subscribe(this);
        Trace::out("FloorLimitSwitchesMonitor: Subscribed to controller - result: %d", subscribeOk);
    }

    // Subscribe limit switches (comme Button)
    bool ok = true;
    ok &= limitSwitch0.registerCallback(
        this, (interface::InputCallbackProvider::CallbackMethod)&FloorLimitSwitchesMonitor::onLimitSwitchChanged);
    ok &= limitSwitch1.registerCallback(
        this, (interface::InputCallbackProvider::CallbackMethod)&FloorLimitSwitchesMonitor::onLimitSwitchChanged);

    Trace::out("FloorLimitSwitchesMonitor: Subscribed to limit switches - result: %d", ok);
}

void FloorLimitSwitchesMonitor::start()
{
    if (!timerActive)
    {
        timerActive = true;
        errorAlreadyNotified = false;
        currentState = ST_WAIT_4_ELEVATOR_2_START;
        Trace::out("FloorLimitSwitchesMonitor: Started");
    }
}

void FloorLimitSwitchesMonitor::stop()
{
    if (timerActive)
    {
        timerActive = false;
        k_timer_stop(&checkTimer);
        Trace::out("FloorLimitSwitchesMonitor: Stopped");
    }
}

void FloorLimitSwitchesMonitor::onElevatorStarted()
{
    Trace::out("FloorLimitSwitchesMonitor: Elevator started");
    startTsMs = (int32_t)k_uptime_get();

    SM_processEvent(evElevatorStarted);

    if (currentState == ST_MONITORING_SWITCHES && timerActive && !errorAlreadyNotified)
    {
        k_timer_start(&checkTimer, K_MSEC(CHECK_INTERVAL_MS), K_MSEC(CHECK_INTERVAL_MS));
    }
}

void FloorLimitSwitchesMonitor::onElevatorReachedFloor(FloorNumber floorNumber)
{
    Trace::out("FloorLimitSwitchesMonitor: Elevator reached floor %d", floorNumber);

    // Check cohérence à l'arrêt
    if (!checkAtStopOk(floorNumber))
    {
        SM_processEvent(evError);
        return;
    }

    SM_processEvent(evElevatorReachedFloor);
    k_timer_stop(&checkTimer);
}

void FloorLimitSwitchesMonitor::onElevatorError()
{
    Trace::out("FloorLimitSwitchesMonitor: Elevator error detected");
    k_timer_stop(&checkTimer);

    errorAlreadyNotified = true;

    if (currentState != ST_SWITCH_ERROR)
    {
        SM_processEvent(evError);
    }
}

void FloorLimitSwitchesMonitor::onLimitSwitchChanged(InputId limitSwitchIndex, bool active)
{
    // Dans ton projet: limitSwitchIndex == floorNumber (0 ou 1)
    if (limitSwitchIndex < 2)
    {
        swActive[limitSwitchIndex] = active;
        // Trace::out("FloorLimitSwitchesMonitor: sw[%d]=%d", limitSwitchIndex, active);
    }
}

uint32_t FloorLimitSwitchesMonitor::countActive() const
{
    uint32_t c = 0;
    for (int i = 0; i < 2; i++)
    {
        if (swActive[i]) c++;
    }
    return c;
}

bool FloorLimitSwitchesMonitor::checkDuringMoveOk()
{
    const uint32_t activeCount = countActive();
    const int32_t nowMs = (int32_t)k_uptime_get();

    // 1) jamais 2 switches actifs
    if (activeCount > 1)
    {
        Trace::out("FloorLimitSwitchesMonitor: ERROR - multiple switches active");
        return false;
    }

    // 2) pendant le mouvement, après une petite grace: aucun switch ne doit rester actif
    if ((nowMs - startTsMs) > START_GRACE_MS)
    {
        if (activeCount == 1)
        {
            Trace::out("FloorLimitSwitchesMonitor: ERROR - switch active while moving");
            return false;
        }
    }

    return true;
}

bool FloorLimitSwitchesMonitor::checkAtStopOk(FloorNumber floorNumber)
{
    const uint32_t activeCount = countActive();

    // À l'arrêt: exactement 1 switch actif
    if (activeCount != 1)
    {
        Trace::out("FloorLimitSwitchesMonitor: ERROR - expected 1 active switch at stop, got %d", activeCount);
        return false;
    }

    // et c'est le bon (floorNumber = 0 ou 1)
    if (floorNumber < 2)
    {
        if (!swActive[floorNumber])
        {
            Trace::out("FloorLimitSwitchesMonitor: ERROR - wrong switch active at stop");
            return false;
        }
    }

    return true;
}

void FloorLimitSwitchesMonitor::SM_processEvent(SMEvents eventId)
{
    const SMStates oldState = currentState;

    switch (currentState)
    {
        case ST_WAIT_4_ELEVATOR_2_START:
        {
            if (eventId == evElevatorStarted)
            {
                currentState = ST_MONITORING_SWITCHES;
            }
            break;
        }

        case ST_MONITORING_SWITCHES:
        {
            if (eventId == evElevatorReachedFloor)
            {
                currentState = ST_WAIT_4_ELEVATOR_2_START;
            }
            else if (eventId == evCheckTimeout)
            {
                if (!checkDuringMoveOk())
                {
                    currentState = ST_SWITCH_ERROR;
                }
            }
            else if (eventId == evError)
            {
                currentState = ST_SWITCH_ERROR;
            }
            break;
        }

        case ST_SWITCH_ERROR:
        default:
            break;
    }

    if (oldState != currentState)
    {
        switch (currentState)
        {
            case ST_WAIT_4_ELEVATOR_2_START:
                Trace::out("FloorLimitSwitchesMonitor: Waiting for elevator to start");
                break;

            case ST_MONITORING_SWITCHES:
                Trace::out("FloorLimitSwitchesMonitor: Monitoring limit switches");
                break;

            case ST_SWITCH_ERROR:
                Trace::out("FloorLimitSwitchesMonitor: LIMIT SWITCH ERROR!");
                if (!errorAlreadyNotified)
                {
                    errorAlreadyNotified = true;
                    notifySecurityIssue(ISSUE_LIMIT_SWITCHES); // recommande: issue dédiée
                }
                break;

            default:
                break;
        }
    }
}

void FloorLimitSwitchesMonitor::timerCallback(struct k_timer * timer)
{
    FloorLimitSwitchesMonitor * pThis = CONTAINER_OF(timer, FloorLimitSwitchesMonitor, checkTimer);
    pThis->handleTimerTimeout();
}

void FloorLimitSwitchesMonitor::handleTimerTimeout()
{
    if (currentState == ST_MONITORING_SWITCHES && !errorAlreadyNotified)
    {
        SM_processEvent(evCheckTimeout);
    }
}

} // namespace security
