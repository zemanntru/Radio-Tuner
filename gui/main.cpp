#include "libraries/graphics.hpp"

int main()
{       
        Graphics::World gui("/dev/ttyUSB0");
        gui.render();
        return 0;
}
