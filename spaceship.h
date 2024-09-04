#pragma once
#include "movable.h"

class SpaceShip : public Movable, public Rotatable {
private:
    Vector position;
    Vector velocity;
    Rotation rotation;

public:
    SpaceShip(const Vector& pos, Rotation rot)
        : position(pos), velocity(Vector()), rotation(rot) {}

    void setVelocity(const Vector& vec) {
        velocity = vec;
    }

    // Implement Movable interface
    Vector getPosition() const override {
        return position;
    }

    Vector getVelocity() const override {
        return velocity;
    }

    Movable& setPosition(const Vector& vector) override {
        position = vector;
        return *this;
    }

    // Implement Rotatable interface
    Rotation getRotation() const override {
        return rotation;
    }

    Rotatable& setRotation(Rotation rot) override {
        rotation = rot;
        return *this;
    }
};
