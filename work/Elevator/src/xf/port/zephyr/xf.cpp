#include <cassert>
#include <config/xf-config.h>

#if (USE_XF_PORT_ZEPHYR_XF_CLASS != 0)

#include "xf/interface/dispatcher.h"
#include "xf/xf.h"

bool XF::isInitialized_ = false;
bool XF::isRunning_ = false;

void XF_initialize(int timeInterval)
{
    XF::initialize(timeInterval);
}

void XF_exec()
{
    XF::exec();
}

void XF_execOnce()
{
    XF::execOnce();
}

void XF::initialize(int timeInterval /* = 10 */, int argc /* = 0 */, char * argv[] /* = nullptr */)
{
    if (!isInitialized_)
    {
        (void)timeInterval;     // Not used
        // TODO: Remove parameter 'timeoutInterval' from method

        isInitialized_ = true;
    }
}

int XF::exec()
{
	isRunning_ = true;     // From here the XF is running

	// Start dispatcher
    interface::XFDispatcher::getDefaultInstance(false)->start();
    // In case the dispatcher is not active, let the main thread execute the dispatcher
    if (!interface::XFDispatcher::getDefaultInstance()->isActive())
    {
	    interface::XFDispatcher::getDefaultInstance()->execute();
    }
    return 0;
}

int XF::execOnce()
{
	static bool usingExecOnce = false;

	if (!usingExecOnce)
	{
		usingExecOnce = true;

		isRunning_ = true;     // From here the XF is running
	}

    // Execute once the default dispatcher
	interface::XFDispatcher::getDefaultInstance()->executeOnce();
    return 1;
}

bool XF::isRunning()
{
    return isRunning_;
}

#endif // USE_XF_PORT_ZEPHYR_XF_CLASS
