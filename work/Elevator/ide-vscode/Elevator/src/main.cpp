#include "xf/xf.h"
#include "app/factory.h"

int main(int argc, char *argv[])
{
    XF::initialize(10, argc, argv);

    app::Factory::initialize();
    app::Factory::build();

    return XF::exec();
}