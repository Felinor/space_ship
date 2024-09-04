#pragma once
#include "vector.h"

class Movable {
public:
    virtual Vector getPosition() const = 0;
    virtual Vector getVelocity() const = 0;
    virtual Movable& setPosition(const Vector& vector) = 0;
    virtual ~Movable() = default;
};

// Rotatable
#pragma once

using Rotation = double;

class Rotatable {
public:
    virtual Rotation getRotation() const = 0;
    virtual Rotatable& setRotation(Rotation rotation) = 0;
    virtual ~Rotatable() = default;
};
