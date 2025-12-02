#include "xf/defaulttransition.h"

XFDefaultTransition::XFDefaultTransition()
 : XFEvent(XFEvent::DefaultTransition, 0)
{
}

bool XFDefaultTransition::deleteAfterConsume() const
{
    return true;
}
