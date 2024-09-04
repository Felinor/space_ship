#pragma once
#include "movable.h"

class RotationHandler {
public:
    static Rotatable& Rotate(Rotatable& rotatable, Rotation angle) {
        rotatable.setRotation(rotatable.getRotation() + angle);
        return rotatable;
    }
};
