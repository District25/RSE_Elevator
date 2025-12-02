#ifndef XF_PORT_QT_CONFIG_H
#define XF_PORT_QT_CONFIG_H

#if (XF_PORT_QT != 0)
    #include <QTimer>
    typedef QTimer * TimeoutHandle;

    #define USE_XF_CORE_BEHAVIOR_CLASS                          1
    #define USE_XF_CORE_DISPATCHER_CLASS                        1
    #define USE_XF_PORT_QT_EVENT_CLASS                          1
    #define USE_XF_PORT_QT_EVENT_QUEUE_CLASS                    1
    #define USE_XF_PORT_QT_MUTEX_CLASS                          1
    #define USE_XF_PORT_QT_THREAD_CLASS                         1
    #define USE_XF_PORT_QT_XF_CLASS                             1
#else
    #warning "XF port Qt config file included, but not activated. Fix: Set XF_PORT_QT define to 1!"
#endif // XF_PORT_QT

#endif // XF_PORT_QT_CONFIG_H
