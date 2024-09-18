#pragma once
#include "spaceship.h"
#include "vector.h"
#include "exception_queue.h"

class ChangeVelocityCommand : public Command {
private:
    SpaceShip& ship;
    Vector newVelocity;

public:
    ChangeVelocityCommand(SpaceShip& ship, const Vector& velocity)
        : ship(ship), newVelocity(velocity) {}

    void Execute() override {
        ship.setVelocity(newVelocity);
    }

    std::string GetName() const override {
        return "ChangeVelocityCommand";
    }
};
