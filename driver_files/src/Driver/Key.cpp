#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#endif

#include "Key.hpp"

namespace Key {

    bool isPressed(KeyEnum key)
    {
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        switch (key) {
            case UP:
                return GetAsyncKeyState(VK_UP) != 0;
            case LEFT:
                return GetAsyncKeyState(VK_LEFT) != 0;
            case DOWN:
                return GetAsyncKeyState(VK_DOWN) != 0;
            case RIGHT:
                return GetAsyncKeyState(VK_RIGHT) != 0;
            case W:
                return GetAsyncKeyState(0x57) != 0;
            case A:
                return GetAsyncKeyState(0x41) != 0;
            case S:
                return GetAsyncKeyState(0x53) != 0;
            case D:
                return GetAsyncKeyState(0x44) != 0;
            case E:
                return GetAsyncKeyState(0x45) != 0;
        }
#else
        (void)key;
        return false;
#endif
    }

}; // namespace Key
