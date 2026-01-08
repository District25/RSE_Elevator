#include "floorlimitswitchesmonitor.h"
#include "trace/trace.h"
#include <zephyr/kernel.h>

namespace security {

FloorLimitSwitchesMonitor::FloorLimitSwitchesMonitor()
    : Monitor(UNKNOWN_ISSUE)   // issue générique (fallback)
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

    // store pointers for polling
    swInput[0] = &limitSwitch0;
    swInput[1] = &limitSwitch1;

    swActive[0] = false;
    swActive[1] = false;

    timerActive = false;
    errorAlreadyNotified = false;
    currentState = ST_WAIT_4_ELEVATOR_2_START;

    pendingIssue = UNKNOWN_ISSUE;

    // Subscribe controller events
    if (controller_)
    {
        bool subscribeOk = controller_->subscribe(this);
        Trace::out("[SW]  Subscribed to controller - result: %d", subscribeOk);
    }

    // initial read (optional)
    updateSwitchStates();
}

void FloorLimitSwitchesMonitor::start()
{
    if (!timerActive)
    {
        timerActive = true;
        errorAlreadyNotified = false;
        currentState = ST_WAIT_4_ELEVATOR_2_START;
        pendingIssue = UNKNOWN_ISSUE;

        Trace::out("[SW]  Started");
    }
}

void FloorLimitSwitchesMonitor::stop()
{
    if (timerActive)
    {
        timerActive = false;
        k_timer_stop(&checkTimer);
        Trace::out("[SW]  Stopped");
    }
}

void FloorLimitSwitchesMonitor::onElevatorStarted()
{
    //Trace::out("[SW]  Elevator started");  
    startTsMs = (int32_t)k_uptime_get();

    pendingIssue = UNKNOWN_ISSUE;

    // refresh states at start
    updateSwitchStates();

    SM_processEvent(evElevatorStarted);

    if (currentState == ST_MONITORING_SWITCHES && timerActive && !errorAlreadyNotified)
    {
        k_timer_start(&checkTimer, K_MSEC(CHECK_INTERVAL_MS), K_MSEC(CHECK_INTERVAL_MS));
    }
}

void FloorLimitSwitchesMonitor::onElevatorReachedFloor(FloorNumber floorNumber)
{
    //Trace::out("Elevator reached floor %d", floorNumber);

    // Update switches right now (because stop check is immediate)
    updateSwitchStates();

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
    Trace::out("[SW]  Elevator error detected");
    k_timer_stop(&checkTimer);

    errorAlreadyNotified = true;

    if (currentState != ST_SWITCH_ERROR)
    {
        SM_processEvent(evError);
    }
}

void FloorLimitSwitchesMonitor::updateSwitchStates()
{
    if (swInput[0]) swActive[0] = swInput[0]->isActive();
    if (swInput[1]) swActive[1] = swInput[1]->isActive();
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
        pendingIssue = ISSUE_LIMIT_SWITCH_MULTIPLE_ACTIVE;
        Trace::out("[SW]  ERROR - multiple switches active (sw0=%d sw1=%d)",
                   swActive[0], swActive[1]);
        return false;
    }

    // 2) pendant le mouvement, après une petite grace: aucun switch ne doit rester actif
    if ((nowMs - startTsMs) > START_GRACE_MS)
    {
        if (activeCount == 1)
        {
            pendingIssue = ISSUE_LIMIT_SWITCH_ACTIVE_WHILE_MOVING;
            Trace::out("[SW]  ERROR - switch active while moving (sw0=%d sw1=%d)",
                       swActive[0], swActive[1]);
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
        pendingIssue = ISSUE_LIMIT_SWITCH_NONE_ACTIVE_AT_STOP;
        Trace::out("[SW]  ERROR - expected 1 active switch at stop, got %d (sw0=%d sw1=%d)",
                   activeCount, swActive[0], swActive[1]);
        return false;
    }

    // et c'est le bon (floorNumber = 0 ou 1)
    if (floorNumber < 2)
    {
        if (!swActive[floorNumber])
        {
            pendingIssue = ISSUE_LIMIT_SWITCH_WRONG_AT_STOP;
            Trace::out("[SW]  ERROR - wrong switch active at stop (floor=%d sw0=%d sw1=%d)",
                       floorNumber, swActive[0], swActive[1]);
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
                // refresh + check
                updateSwitchStates();

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
                //Trace::out("[SW]  Waiting for elevator to start");
                break;

            case ST_MONITORING_SWITCHES:
                //Trace::out("[SW]  Monitoring limit switches");
                break;

            case ST_SWITCH_ERROR:
                //Trace::out("[SW]  LIMIT SWITCH ERROR! issue=%d", (int)pendingIssue);
                if (!errorAlreadyNotified)
                {
                    errorAlreadyNotified = true;

                    if (pendingIssue == UNKNOWN_ISSUE)
                    {
                        notifySecurityIssue(UNKNOWN_ISSUE);
                    }
                    else
                    {
                        notifySecurityIssue(pendingIssue);
                    }
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
        // update + check on each tick
        updateSwitchStates();
        SM_processEvent(evCheckTimeout);
    }
}

} // namespace security
