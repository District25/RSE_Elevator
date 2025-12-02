
INCLUDEPATH += $$PWD/..

# Path to qt based implementation of the XF port
PORT_QT_PATH = $$PWD/../qt

INCLUDEPATH += \
    "$${PORT_QT_PATH}"

HEADERS += \
    "$${PORT_QT_PATH}/eventqueue.h" \
    "$${PORT_QT_PATH}/mutex.h" \
    "$${PORT_QT_PATH}/thread.h" \
    "$${PORT_QT_PATH}/timeoutmanager.h" \
    "$${PORT_QT_PATH}/config/xf-port-config.h"

SOURCES += \
    "$${PORT_QT_PATH}/eventqueue.cpp" \
    "$${PORT_QT_PATH}/event.cpp" \
    "$${PORT_QT_PATH}/mutex.cpp" \
    "$${PORT_QT_PATH}/thread.cpp" \
    "$${PORT_QT_PATH}/xf.cpp" \
    "$${PORT_QT_PATH}/timeoutmanager.cpp"
