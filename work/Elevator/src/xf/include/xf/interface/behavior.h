#ifndef XF_INTERFACE_BEHAVIOR_H
#define XF_INTERFACE_BEHAVIOR_H

#include <string>
#include "xf/event.h"

class XFDispatcher;
class XFDispatcherPort;

namespace interface {

/**
 * @brief Interface to receive and process events.
 *
 * Interface that needs to be implemented by every class having a
 * behavior. Interface methods are needed by the other parts of the XF.
 *
 * This interface is used by the dispatcher to process events.
 */
class XFBehavior
{
    // Only XFDispatchers should get access to the 'process' method
    friend class ::XFDispatcher;
    friend class ::XFDispatcherPort;

public:
    using TerminateBehavior = bool;                                     ///< Type returned by the process() method.

public:
    virtual ~XFBehavior() = default;

    virtual void startBehavior() = 0;                               	///< Starts the behavior.
    virtual void pushEvent(XFEvent * pEvent, uint32_t timeout = 0) = 0;                 ///< Pushs an event to be processed by the behavior.
    virtual XFEvent::XFEventHandle pushEvent(int eventId, uint32_t timeout = 0) = 0;    ///< Method to create and push and event.
    virtual bool deleteOnTerminate() const = 0;                     	///< Tells XF to delete behavior when receiving terminate event.
    virtual void setDeleteOnTerminate(bool deleteBehavior) = 0;   		///< Sets/Clears the 'delete on terminate' property.

protected:
    virtual void cancelEvent(XFEvent::XFEventHandle eventHandle) = 0;   ///< Cancels a delayed event for this state machine.
    virtual TerminateBehavior process(const XFEvent * pEvent) = 0;  	///< Called by the dispatcher to process an event.

protected:
    XFBehavior() = default;
};

} // namespace interface
#endif // XF_INTERFACE_BEHAVIOR_H
