#pragma once
#include "spaceship.h"
#include "exception_queue.h"
#include <stdexcept>

class BurnFuelCommand : public Command {
private:
    SpaceShip& ship;
    double fuelToBurn;

public:
    BurnFuelCommand(SpaceShip& ship, double fuel) : ship(ship), fuelToBurn(fuel) {}

    void Execute() override {
        ship.burnFuel(fuelToBurn);
    }

    std::string GetName() const override {
        return "BurnFuelCommand";
    }
};
