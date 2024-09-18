#pragma once
#include <cmath>
#include "spaceship.h"
#include "exception_queue.h"

class RotateAndChangeVelocity : public Command {
private:
    SpaceShip& ship;
    Rotation angle;
    Vector newVelocity;

public:
    RotateAndChangeVelocity(SpaceShip& ship, Rotation angle, const Vector& velocity)
            : ship(ship), angle(angle), newVelocity(velocity) {}

    void Execute() override {
        // Поворот корабля
        ship.setRotation(ship.getRotation() + angle);

        // Проверка: если объект движется (скорость ненулевая), изменяем вектор скорости
        if (ship.getVelocity() != Vector(0, 0)) {
            // Получаем текущий вектор скорости
            Vector velocity = ship.getVelocity();
            // Применяем поворот к вектору скорости
            Vector newVelocity = RotateVector(velocity, angle);
            // Устанавливаем новый вектор скорости кораблю
            ship.setVelocity(newVelocity);
        }
    }

    std::string GetName() const override {
        return "RotateAndChangeVelocity";
    }

private:
    // Простой расчет скорости на основе угла поворота
    Vector RotateVector(const Vector& velocity, Rotation angle) {
        double radians = angle * M_PI / 180.0;
        double newX = velocity.X * cos(radians) - velocity.Y * sin(radians);
        double newY = velocity.X * sin(radians) + velocity.Y * cos(radians);
        return Vector(newX, newY);
    }
};
