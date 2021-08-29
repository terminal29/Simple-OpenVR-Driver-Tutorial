#pragma once

namespace Key {

    enum KeyEnum {
        UP,
        LEFT,
        DOWN,
        RIGHT,
        W,
        A,
        S,
        D,
        E,
    };

    bool isPressed(KeyEnum key);

}; // namespace Key
