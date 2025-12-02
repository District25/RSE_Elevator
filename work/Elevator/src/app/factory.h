#ifndef APP_FACTORY_H
#define APP_FACTORY_H

//
// What is seen only by the C++ compiler
//
#ifdef __cplusplus

namespace elevator { class Controller; class Button; }
namespace io { class Input; }
namespace motor { class Driver; class Decoder; }
namespace security { class Factory; }
namespace board { class ButtonsController; class LedsController; }

namespace app
{

/**
 * @brief Application factory responsible to create needed objects.
 */
class Factory
{
    friend class security::Factory;
public:
    Factory() = delete;

    static void initialize();           ///< Initializes the factory
    static void build();                ///< Creates components and initializes relations

protected:
    static io::Input & limitSwitchFloor1();
    static io::Input & limitSwitchFloor2();
    static board::ButtonsController & buttonsController();
    static board::LedsController & ledsController();
    static motor::Driver & motorDriver();
    static motor::Decoder & motorDecoder();
    static elevator::Controller & controller();
    static elevator::Button & buttonFloor1();
    static elevator::Button & buttonFloor2();
    static elevator::Button & buttonElevatorDown();
    static elevator::Button & buttonElevatorUp();
};

} /* namespace app */
#endif // __cplusplus

//
// What is seen by the C and C++ compiler
//
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void Factory_initialize();
void Factory_build();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // APP_FACTORY_H
