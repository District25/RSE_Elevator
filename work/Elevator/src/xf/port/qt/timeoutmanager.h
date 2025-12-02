#ifndef XF_PORT_QT_TIMEOUTMANAGER_H
#define XF_PORT_QT_TIMEOUTMANAGER_H

#include <QThread>
#include "config/xf-config.h"

class XFEvent;

/** @ingroup port_qt
 *  @{
 */

/**
 * @brief Used to handle QTimers from other threads.
 */
class XFTimeoutManager : public QThread
{
    Q_OBJECT
public:
    static XFTimeoutManager & instance();                               ///< Returns a reference to the single instance.

    ~XFTimeoutManager();

    void delayEvent(XFEvent * event, int32_t timeoutInMilliseconds);    ///< Delays the event about given timeout in milliseconds.
    void stopTimer(TimeoutHandle timer);                                ///< Stops/cancels the timer with the given TimeoutHandle.
    void purgeTimer(TimeoutHandle timer);                               ///< Delete the timer with the given TimeoutHandle.

signals:
    void sigDelayEvent(TimeoutHandle timer, int32_t timeoutInMilliseconds, XFEvent * event);
    void sigStopTimer(TimeoutHandle timer);
    void sigPurgeTimer(TimeoutHandle timer);

protected slots:
    void onDelayEvent(TimeoutHandle timer, int32_t timeoutInMilliseconds, XFEvent * event);
    void onStopTimer(TimeoutHandle timer);
    void onPurgeTimer(TimeoutHandle timer);

protected:
    XFTimeoutManager();

    void delayEvent_(TimeoutHandle timer, int32_t timeoutInMilliseconds, XFEvent * event);  ///< Internal method to handle delayed event.

    // QThread interface
protected:
    void run() override;                        ///< QThread method override.

protected:
    static XFTimeoutManager * instance_;        ///< Pointer to single instance.
};

/** @} */ // end of port_qt group
#endif // XF_PORT_QT_TIMEOUTMANAGER_H
