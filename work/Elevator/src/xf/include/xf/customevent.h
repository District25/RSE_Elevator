#ifndef XF_CUSTOMEVENT_H
#define XF_CUSTOMEVENT_H

#include "xf/event.h"

/** @ingroup xf_core
 *  @{
 */

/**
 * @brief Base class for project related events.
 *
 * It is possible to create custom events by deriving new classes/events from
 * this class.
 * All custom events have `#Event` as event type. In the state machine the event
 * can be uniquely identified with `getEventType() == Event` and the event id
 * `getId()` given to the object at construction time.
 *
 * It is up to the developer to handle the different event ids.
 */
class XFCustomEvent : public XFEvent
{
public:
    /**
     * Constructor for the class
     *
     * @param id Event identifier to identify the custom event (in the context of the behavior).
     * @param pBehavior The behavior for which the event was constructed (will consume the event).
     **/
    XFCustomEvent(EventId id, bool deleteAfterConsume = true, interface::XFBehavior * pBehavior = nullptr);

    /**
     * @brief Tells the XF if the event should be deleted after processing.
     * @return True if the event should be deleted.
     */
    bool deleteAfterConsume() const override { return bDeleteAfterConsume_; }

protected:
    /**
     * @brief Setter for #bDeleteAfterConsume_ attribute.
     * @param bDeleteAfterConsume Parameter to be assigned. Default: true.
     */
    void setDeleteAfterConsume(bool bDeleteAfterConsume = true) { bDeleteAfterConsume_ = bDeleteAfterConsume; }

protected:
    bool bDeleteAfterConsume_;          ///< True if the XF should delete event after consumption/dispatching.
};

/** @} */ // end of xf_core group
#endif // XF_CUSTOMEVENT_H
