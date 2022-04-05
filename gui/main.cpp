#include "libraries/graphics.hpp"

Graphics::World gui("/dev/ttyUSB0");

int main()
{       
        gui.render();
        return 0;
}
