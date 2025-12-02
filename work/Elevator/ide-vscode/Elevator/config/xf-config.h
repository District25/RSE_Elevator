#ifndef XF_CONFIG_H
#define XF_CONFIG_H

/**
 * Info: Only one XF PORT should be enabled at a time!
 *
 * Ports available:
 *   - XF_PORT_ZEPHYR
 *   - XF_PORT_QT
 *
 * Note: Define one of the defines above in your project configuration.
 */

#include "config/xf-port-config.h"

#if defined(XF_PORT_ZEPHYR)
    #define XF_PORT_ZEPHYR_ENABLED      1
#endif

#if defined(XF_PORT_QT)
    #define XF_PORT_QT_ENABLED          1
#endif

#if !defined(XF_PORT_ZEPHYR) &&             \
    !defined(XF_PORT_QT)
    #error "No XF port activated!"
#endif

#endif // XF_CONFIG_H
