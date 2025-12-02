#ifndef EV_GENERIC_H
#define EV_GENERIC_H

#include "xf/customevent.h"

/**
 * @brief Generic event holding three individual attributes.
 */
template <class AttrT1 = void, class AttrT2 = void, class AttrT3 = void> 
class evGeneric : public XFCustomEvent
{
public:
    AttrT1 attribute1;
    AttrT2 attribute2;
    AttrT3 attribute3;

public:
    evGeneric(EventId id, AttrT1 attribute1, AttrT2 attribute2, AttrT3 attribute3) :
        XFCustomEvent(id),
        attribute1(attribute1),
        attribute2(attribute2),
        attribute3(attribute3)
    {}
};

/**
 * @brief Generic event holding two individual attributes.
 */
template <class AttrT1, class AttrT2>
class evGeneric<AttrT1, AttrT2> : public XFCustomEvent
{
public:
    AttrT1 attribute1;
    AttrT2 attribute2;

public:
    evGeneric(EventId id, AttrT1 attribute1, AttrT2 attribute2) :
        XFCustomEvent(id),
        attribute1(attribute1),
        attribute2(attribute2)
    {}
};

/**
 * @brief Generic event holding one individual attribute.
 */
template <class AttrT1>
class evGeneric<AttrT1> : public XFCustomEvent
{
public:
    AttrT1 attribute1;

public:
    evGeneric(EventId id, AttrT1 attribute1) :
        XFCustomEvent(id),
        attribute1(attribute1)
    {}
};

#endif // EV_GENERIC_H
