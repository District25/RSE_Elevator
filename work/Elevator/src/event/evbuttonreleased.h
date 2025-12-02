#ifndef EVBUTTONRELEASED_H
#define EVBUTTONRELEASED_H

#include "xf/customevent.h"
#include "events.h"

/**
 * @brief Event send when button is released.
 *
 * Used internally in state-machine handling one particular button.
 */
class evButtonReleased : public XFCustomEvent
{
public:
    evButtonReleased();
};

#endif // EVBUTTONRELEASED_H
