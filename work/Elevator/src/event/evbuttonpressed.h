#ifndef EVBUTTONPRESSED_H
#define EVBUTTONPRESSED_H

#include "xf/customevent.h"
#include "events.h"

/**
 * @brief Event send when button is pressed.
 *
 * Used internally in state-machine handling one particular button.
 */
class evButtonPressed : public XFCustomEvent
{
public:
    evButtonPressed();
};

#endif // EVBUTTONPRESSED_H
