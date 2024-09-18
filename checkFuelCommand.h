#pragma once
#include "spaceship.h"
#include "exception_queue.h"
#include <stdexcept>

class CheckFuelCommand : public Command {
private:
    SpaceShip& ship;
    double requiredFuel;

public:
    CheckFuelCommand(SpaceShip& ship, double fuel) : ship(ship), requiredFuel(fuel) {}

    void Execute() override {
        if (ship.getFuel() < requiredFuel) {
            throw std::runtime_error("Not enough fuel to execute the command.");
        }
    }

    std::string GetName() const override {
        return "CheckFuelCommand";
    }
};
