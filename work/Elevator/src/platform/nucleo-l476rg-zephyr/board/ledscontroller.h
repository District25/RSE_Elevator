#ifndef BOARD_LEDSCONTROLLER_H
#define BOARD_LEDSCONTROLLER_H

#include <cassert>
#include <cstdint>

namespace board {

/**
 * @brief LED controller class to enable/disable one ore more LEDs.
 * 
 */
class LedsController
{
public:
	static constexpr const uint32_t LED_COUNT = 8;

public:
	LedsController();
	virtual ~LedsController();

	bool initialize();

	inline static LedsController & getInstance() { assert(instance); return *instance; }

	void setLed(uint8_t index, bool bOn = true);
	void setLeds(uint8_t ledMask, bool bOn = true);

	inline uint8_t ledCount() const { return LED_COUNT; }

	void setLed0(bool bOn = true);
	void setLed1(bool bOn = true);
	void setLed2(bool bOn = true);
	void setLed3(bool bOn = true);
	void setLed4(bool bOn = true);
	void setLed5(bool bOn = true);
	void setLed6(bool bOn = true);
	void setLed7(bool bOn = true);

protected:
	typedef void (LedsController::*ledMethod)(bool bOn);	///< Function prototype to led operation. Used for fast access to LED operation.

protected:
	static LedsController * instance;

	ledMethod ledOperation_[LED_COUNT];		///< Array of pointers to led functions.
};

} // namespace board
#endif // BOARD_LEDSCONTROLLER_H
