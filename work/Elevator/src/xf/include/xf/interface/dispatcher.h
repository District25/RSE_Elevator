#ifndef XF_INTERFACE_DISPATCHER_H
#define XF_INTERFACE_DISPATCHER_H

#include "xf/event.h"

class XF;
class DispatcherThread;

namespace interface {

class XFBehavior;

/**
 * @brief Interface for the XF dispatcher providing the event processing loop.
 *
 * Depending on the implementation the dispatcher is executed by
 * an internal thread. It is up to the port implementation how
 * the class behaves.
 *
 * The dispatcher should at least provide a queue to hold events
 * and an algorithm (typically in execute()) that dispatches the queued
 * events to the associated behavior (eq. state machine, activity).
 *
 * There may exist multiple dispatchers. Each behavior needs to be
 * bound to a dispatcher. One dispatcher may serve different behaviors.
 *
 */
class XFDispatcher
{
    friend class ::XF;
    friend class ::DispatcherThread;

public:
    using XFEventHandle = XFEvent::XFEventHandle;                         ///< Handle to XFEvent. Used to identify a delayed event.

    virtual ~XFDispatcher() = default;

    static XFDispatcher * getDefaultInstance(bool isActive = true);       ///< Returns a pointer to the default instance of XFDispatcher.

    static XFDispatcher * create(const char * threadName = nullptr);      ///< Method to be implemented by the derived class (providing objects of its class).

    virtual void pushEvent(XFEvent * pEvent, uint32_t timeout = 0) = 0;   ///< Adds event to the events queue.

    /**
     * @brief Adds a new event to be handled.
     *
     * The dispatcher pushes a new event using the information provided.
     */
    virtual XFEventHandle pushEvent(int eventId, uint32_t timeout, interface::XFBehavior * pBehavior) = 0;

    virtual bool isActive() const = 0;          ///< Returns true if the internal thread is executing the dispatcher.
    virtual void start() = 0;                   ///< Starts the dispatcher in case it has an internal thread.

protected:
    /**
     * Constructor is protected because only method createInstance()
     * should be called to create Dispatcher instances
     */
    XFDispatcher() = default;

    /**
     * @brief Main loop of the dispatcher. Implements event loop processing.
     */
    virtual int execute(const void * param = nullptr) = 0;

    /**
     * @brief Executes once the dispatcher.
     *
     * Usually, this method must not be called explicitly. The
     * standard way to call the dispatcher is to call execute().
     *
     * This method can be called by the Thread (or main function) if it
     * must perform concurrent (non-XF related) tasks in addition.
     *
     * When using this method start() must not be called.
     */
    virtual void executeOnce() = 0;

    /**
     * @brief Dispatches the event to the corresponding behavioral part.
     *
     * For example the state machine which should process the event.
     *
     * \param pEvent The event to dispatch
     */
    virtual void dispatchEvent(const XFEvent * pEvent) const = 0;
};

} // namespace interface
#endif // XF_INTERFACE_DISPATCHER_H
