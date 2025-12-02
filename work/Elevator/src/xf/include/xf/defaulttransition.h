#ifndef XF_DEFAULTTRANSITION_H
#define XF_DEFAULTTRANSITION_H

#include "xf/event.h"

/** @ingroup xf_core
 *  @{
 */

/**
 * @brief State machine transition without trigger.
 *
 * Represents a transition in a state machine having no trigger.
 *
 * In case one wants to directly transition from one state to
 * another (without a trigger) a default transition must be pushed.
 */
class XFDefaultTransition : public XFEvent
{
public:
    /**
     * Constructor
     */
    XFDefaultTransition();

    bool deleteAfterConsume() const override;   ///< See XFEvent.
};

/** @} */ // end of xf_core group
#endif // XF_DEFAULTTRANSITION_H
