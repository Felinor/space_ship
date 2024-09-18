#pragma once
#include "movement.h"
#include "burnFuelCommand.h"
#include "checkFuelCommand.h"

class MoveWithFuelCommand : public Command {
private:
    SpaceShip& ship;
    double fuelNeeded;

public:
    MoveWithFuelCommand(SpaceShip& ship, double fuelNeeded) : ship(ship), fuelNeeded(fuelNeeded) {}

    void Execute() override {
        CheckFuelCommand checkFuel(ship, fuelNeeded);
        checkFuel.Execute();  // Проверка топлива

        BurnFuelCommand burnFuel(ship, fuelNeeded);
        burnFuel.Execute();  // Сжигаем топливо

        Movement::Move(ship);  // Перемещение
    }

    std::string GetName() const override {
        return "MoveWithFuelCommand";
    }
};
