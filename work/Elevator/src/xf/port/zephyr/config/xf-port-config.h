#ifndef XF_PORT_ZEPHYR_CONFIG_H
#define XF_PORT_ZEPHYR_CONFIG_H

#if (XF_PORT_ZEPHYR != 0)
    #include <cstdint>
    typedef uint32_t TimeoutHandle;

    #define USE_XF_CORE_BEHAVIOR_CLASS                          1
    #define USE_XF_CORE_DISPATCHER_CLASS                        1
    #define USE_XF_PORT_ZEPHYR_XF_CLASS                         1
    #define USE_XF_PORT_ZEPHYR_EVENT_CLASS                      1
    #define USE_XF_PORT_ZEPHYR_EVENT_QUEUE_CLASS                1
    #define USE_XF_PORT_ZEPHYR_MUTEX_CLASS                      1
    #define USE_XF_PORT_ZEPHYR_THREAD_CLASS                     1
#else
    #warning "XF port Zephyr config file included, but not activated. Fix: Set XF_PORT_ZEPHYR define to 1!"    
#endif // XF_PORT_ZEPHYR

#endif // XF_PORT_ZEPHYR_CONFIG_H
