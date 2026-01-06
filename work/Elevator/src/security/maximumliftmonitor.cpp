#include "maximumliftmonitor.h"
#include "trace/trace.h"
#include <zephyr/kernel.h>

namespace security {

    // Petit helper local
    static constexpr int32_t POS_TOL = 15; // tolérance anti-jitter / bruit (à ajuster)
    static constexpr int32_t CHECK_INTERVAL_MS = 200; // si c'est déjà dans le .h, enlève cette ligne

    MaximumLiftMonitor::MaximumLiftMonitor()
        : Monitor(ISSUE_MAXIMUM_LIFT_EXCEEDED)
    {
        k_timer_init(&checkTimer, &MaximumLiftMonitor::timerCallback, nullptr);
    }

    MaximumLiftMonitor::~MaximumLiftMonitor()
    {
        stop();
    }

    void MaximumLiftMonitor::initialize(elevator::Controller & controller, motor::Decoder & decoder)
    {
        controller_ = &controller;
        decoder_ = &decoder;

        limitsCalibrated = false;
        minPos = 0;
        maxPos = 0;

        bottomCaptured = false;
        topCaptured = false;
        bottomCapturePos = 0;
        topCapturePos = 0;

        // Subscribe to elevator controller events
        if (controller_)
        {
            bool subscribeOk = controller_->subscribe(this);
            Trace::out("MaximumLiftMonitor: Subscribed to controller - result: %d", subscribeOk);
        }
    }

    void MaximumLiftMonitor::start()
    {
        if (!timerActive)
        {
            timerActive = true;
            errorAlreadyNotified = false;
            currentState = ST_WAIT_4_ELEVATOR_2_START;

            Trace::out("MaximumLiftMonitor: Started");
        }
    }

    void MaximumLiftMonitor::stop()
    {
        if (timerActive)
        {
            timerActive = false;
            k_timer_stop(&checkTimer);
            Trace::out("MaximumLiftMonitor: Stopped");
        }
    }

    void MaximumLiftMonitor::onElevatorStarted()
    {
        Trace::out("MaximumLiftMonitor: Elevator started - starting to monitor position");
        SM_processEvent(evElevatorStarted);

        // Limits not acquired, we do not monitor
        if (!limitsCalibrated)
        {
            Trace::out("MaximumLiftMonitor: Limits NOT locked yet -> monitoring paused");
            return;
        }

        // Start timer when elevator starts moving
        if (currentState == ST_MONITORING_ELEVATOR && timerActive && !errorAlreadyNotified)
        {
            k_timer_start(&checkTimer, K_MSEC(CHECK_INTERVAL_MS), K_MSEC(CHECK_INTERVAL_MS));
        }
    }

    void MaximumLiftMonitor::onElevatorReachedFloor(FloorNumber floorNumber)
    {
        Trace::out(">>> MaximumLiftMonitor::onElevatorReachedFloor CALLED with floor %d", floorNumber);

        if (decoder_)
        {
            const int32_t pos = decoder_->getPosition();

            // Calibration of the limits only on start
            if (!limitsCalibrated)
            {
                if (floorNumber == 0 && !bottomCaptured)
                {
                    bottomCaptured = true;
                    bottomCapturePos = pos;
                    Trace::out("MaximumLiftMonitor: Captured BOTTOM position=%ld", bottomCapturePos);
                }
                else if (floorNumber == 1 && !topCaptured)
                {
                    topCaptured = true;
                    topCapturePos = pos;
                    Trace::out("MaximumLiftMonitor: Captured TOP position=%ld", topCapturePos);
                }

                if (bottomCaptured && topCaptured)
                {
                    if (bottomCapturePos < topCapturePos)
                    {
                        minPos = bottomCapturePos;
                        maxPos = topCapturePos;
                    }
                    else
                    {
                        minPos = topCapturePos;
                        maxPos = bottomCapturePos;
                    }

                    limitsCalibrated = (minPos < maxPos);

                    Trace::out("MaximumLiftMonitor: LIMITS LOCKED min=%ld max=%ld", minPos, maxPos);
                }
                else
                {
                    Trace::out("MaximumLiftMonitor: Waiting for other endstop (bottom=%d top=%d)",
                               bottomCaptured, topCaptured);
                }
            }
        }

        SM_processEvent(evElevatorReachedFloor);

        // Stop timer when elevator reaches floor
        k_timer_stop(&checkTimer);
    }

    void MaximumLiftMonitor::onElevatorError()
    {
        if (currentState != ST_LIFT_ERROR)
        {
            SM_processEvent(evError);
        }
    }

    bool MaximumLiftMonitor::checkElevatorPositionOk()
    {
        if (!decoder_)
            return false;

        // Tant que pas locké: on ne déclenche rien
        if (!limitsCalibrated)
        {
            Trace::out("MaximumLiftMonitor: Skipping check (limits not locked)");
            return true;
        }

        const int32_t currentPos = decoder_->getPosition();
        const motor::Driver::Direction dir = decoder_->getDirection();

        Trace::out("MaximumLiftMonitor: pos=%ld dir=%d (min=%ld max=%ld tol=%ld)",
                   currentPos, (int)dir, minPos, maxPos, (long)POS_TOL);

        // Check directionnel : on arrête seulement si dépassement EN HAUT ou EN BAS
        if (dir == motor::Driver::FORWARD) // montée (à confirmer dans ton driver)
        {
            if (currentPos > (maxPos + POS_TOL))
            {
                Trace::out("MaximumLiftMonitor: TOP exceeded! %ld > %ld",
                           currentPos, maxPos + POS_TOL);
                return false;
            }
        }
        else if (dir == motor::Driver::BACKWARD) // descente
        {
            if (currentPos < (minPos - POS_TOL))
            {
                Trace::out("MaximumLiftMonitor: BOTTOM exceeded! %ld < %ld",
                           currentPos, minPos - POS_TOL);
                return false;
            }
        }
        else
        {
            // Direction inconnue -> check soft (mais toujours avec tolérance)
            if (currentPos < (minPos - POS_TOL) || currentPos > (maxPos + POS_TOL))
            {
                Trace::out("MaximumLiftMonitor: Limit exceeded (unknown dir)");
                return false;
            }
        }

        return true;
    }

    void MaximumLiftMonitor::SM_processEvent(SMEvents eventId)
    {
        SMStates oldState = currentState;

        switch (currentState)
        {
            case ST_WAIT_4_ELEVATOR_2_START:
            {
                if (eventId == evElevatorStarted)
                {
                    currentState = ST_MONITORING_ELEVATOR;
                }
                break;
            }

            case ST_MONITORING_ELEVATOR:
            {
                if (eventId == evElevatorReachedFloor)
                {
                    currentState = ST_WAIT_4_ELEVATOR_2_START;
                }
                else if (eventId == evCheckPositionTimeout)
                {
                    if (!checkElevatorPositionOk())
                    {
                        currentState = ST_LIFT_ERROR;
                    }
                }
                else if (eventId == evError)
                {
                    currentState = ST_LIFT_ERROR;
                }
                break;
            }

            case ST_LIFT_ERROR:
            default:
                break;
        }

        if (oldState != currentState)
        {
            switch (currentState)
            {
                case ST_WAIT_4_ELEVATOR_2_START:
                    Trace::out("MaximumLiftMonitor: En attente que l'ascenseur bouge");
                    break;

                case ST_MONITORING_ELEVATOR:
                    Trace::out("MaximumLiftMonitor: Surveillance de l'ascenseur en cours");
                    break;

                case ST_LIFT_ERROR:
                    Trace::out("MaximumLiftMonitor: ERROR");
                    if (!errorAlreadyNotified)
                    {
                        errorAlreadyNotified = true;
                        notifySecurityIssue(ISSUE_MAXIMUM_LIFT_EXCEEDED);
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void MaximumLiftMonitor::timerCallback(struct k_timer * timer)
    {
        MaximumLiftMonitor * pThis = CONTAINER_OF(timer, MaximumLiftMonitor, checkTimer);
        pThis->handleTimerTimeout();
    }

    void MaximumLiftMonitor::handleTimerTimeout()
    {
        if (currentState == ST_MONITORING_ELEVATOR && !errorAlreadyNotified)
        {
            SM_processEvent(evCheckPositionTimeout);
        }
    }

} // namespace security
