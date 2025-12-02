#ifndef INTERFACE_INPUT_CALLBACK_CALLER_H
#define INTERFACE_INPUT_CALLBACK_CALLER_H

#include "inputcallbackprovider.h"

namespace interface {

/**
 * @brief Interface to be provided by the observed class.
 *
 */
class InputCallbackCaller
{
protected:
    virtual ~InputCallbackCaller() = default;

    /**
     * @brief Registers a callback method with its called pointer (callback provider).
     *
     * @return Returns true if the callback provider could be registered, otherwise false.
     */
    virtual bool registerCallback(InputCallbackProvider * callbackProvider,
                                  InputCallbackProvider::CallbackMethod callbackMethod) = 0;

protected:
    InputCallbackCaller() = default;    ///< Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_INPUT_CALLBACK_CALLER_H
