#include <cassert>
#include <QCoreApplication>
#include <config/xf-config.h>

#if (USE_XF_PORT_QT_XF_CLASS != 0)

#include "xf/interface/dispatcher.h"
#include "timeoutmanager.h"
#include "xf/xf.h"

bool XF::isInitialized_ = false;
bool XF::isRunning_ = false;

// Qt application instance used to run signals, slots and timeouts (QTimer)
static QCoreApplication & getApplication(int argc = 0, char * argv[] = nullptr)
{
    static QCoreApplication app(argc, argv);
    return app;
}

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
    (void)argc; (void)argv;

    if (!isInitialized_)
    {
        (void)timeInterval;     // Not used
        // TODO: Remove parameter 'timeoutInterval' from method

        qRegisterMetaType<TimeoutHandle>("TimeoutHandle");
        qRegisterMetaType<int32_t>("int32_t");

        // Call getApplication() to create QT application instance
        ::getApplication(argc, argv);

        XFTimeoutManager::instance().start();

        isInitialized_ = true;
    }
}

int XF::exec()
{
    isRunning_ = true;     // From here the XF is running

    // Start dispatcher
    interface::XFDispatcher::getDefaultInstance(true)->start();

    // Start Qt event loop
    return ::getApplication().exec();
}

int XF::execOnce()
{
    assert(false);    // Not applicable for this port
    return 1;
}

bool XF::isRunning()
{
    return isRunning_;
}

#endif // USE_XF_PORT_QT_XF_CLASS
