#pragma once
#include "movable.h"

class Movement {
public:
    static Movable& Move(Movable& movable) {
        movable.setPosition(movable.getPosition() + movable.getVelocity());
        return movable;
    }
};
