#ifndef INTERFACE_INPUT_CALLBACK_PROVIDER_H
#define INTERFACE_INPUT_CALLBACK_PROVIDER_H

#include <cstdint>

namespace io { class Input; }

namespace interface {

/**
 * @brief Interface used by the observed class to notify a change to the observer using the method callback.
 *
 */
class InputCallbackProvider
{
public:
    using InputId = uint32_t;

    virtual ~InputCallbackProvider() = default;

    /**
     * @brief The method prototype of the callback method to be provided by the class implementing the interface.
     *
     * Example implementation:
     *   // Called by Input to notify active/inactive events.
     *   void onInputChanged(InputId inputId, bool active);
     *
     */
    typedef void (InputCallbackProvider::*CallbackMethod)(InputId inputId, bool active);

protected:
    InputCallbackProvider() = default;      ///< Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_INPUT_CALLBACK_PROVIDER_H
