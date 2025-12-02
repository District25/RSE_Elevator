#ifndef BOARD_HAL_BUTTONS_H
#define BOARD_HAL_BUTTONS_H

#include <cstdint>

/**
 * @brief Button hardware abstraction layer.
 * 
 * Interfaces access to external buttons. The interface uses internally the Zepyhr device tree to access the buttons.
 */

namespace board {
namespace hal {
namespace buttons {

    bool initialize();
    bool isButtonPressed(uint16_t buttonIndex);

} // namespace buttons
} // hal
} // board
#endif // BOARD_HAL_BUTTONS_H
