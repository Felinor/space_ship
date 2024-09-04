#include <iostream>
#include <stdexcept>
#include <functional>
#include "spaceship.h"
#include "movement.h"

// Проверяем результат теста
void assertEquals(const Vector& a, const Vector& b, const std::string& testName) {
    if (!(a == b)) {
        std::cerr << "Test " << testName << " failed: expected " << a << ", got " << b << std::endl;
    } else {
        std::cout << "Test " << testName << " passed." << std::endl;
    }
}

void assertThrows(std::function<void()> func, const std::string& testName) {
    try {
        func();
        std::cerr << "Test " << testName << " failed: expected an exception." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Test " << testName << " passed: caught exception \"" << e.what() << "\"." << std::endl;
    } catch (...) {
        std::cerr << "Test " << testName << " failed: caught unknown exception." << std::endl;
    }
}

// Тест на корректное перемещение объекта
void testMoveChangesPositionCorrectly() {
    SpaceShip ship(Vector(12, 5), 0.0);
    ship.setVelocity(Vector(-7, 3));

    Movement::Move(ship);

    assertEquals(ship.getPosition(), Vector(5, 8), "MoveChangesPositionCorrectly");
}

// Объект с ошибкой при попытке прочитать положение
class FaultyPositionShip : public Movable {
public:
    Vector getPosition() const override {
        throw std::runtime_error("Cannot read position");
    }
    Vector getVelocity() const override {
        return Vector(0, 0);
    }
    Movable& setPosition(const Vector&) override {
        return *this;
    }
};

// Ошибка чтения позиции
void testMoveThrowsOnPositionReadError() {
    FaultyPositionShip ship;
    assertThrows([&]() { Movement::Move(ship); }, "MoveThrowsOnPositionReadError");
}

// Объект с ошибкой при попытке прочитать скорость
class FaultyVelocityShip : public Movable {
public:
    Vector getPosition() const override {
        return Vector(0, 0);
    }
    Vector getVelocity() const override {
        throw std::runtime_error("Cannot read velocity");
    }
    Movable& setPosition(const Vector&) override {
        return *this;
    }
};

// Ошибка чтения скорости
void testMoveThrowsOnVelocityReadError() {
    FaultyVelocityShip ship;
    assertThrows([&]() { Movement::Move(ship); }, "MoveThrowsOnVelocityReadError");
}

// Объект с ошибкой при попытке изменить положение
class FaultySetPositionShip : public Movable {
public:
    Vector getPosition() const override {
        return Vector(0, 0);
    }
    Vector getVelocity() const override {
        return Vector(1, 1);
    }
    Movable& setPosition(const Vector&) override {
        throw std::runtime_error("Cannot set position");
    }
};

// Ошибка установки нового положения
void testMoveThrowsOnSetPositionError() {
    FaultySetPositionShip ship;
    assertThrows([&]() { Movement::Move(ship); }, "MoveThrowsOnSetPositionError");
}

int main() {
    testMoveChangesPositionCorrectly();
    testMoveThrowsOnPositionReadError();
    testMoveThrowsOnVelocityReadError();
    testMoveThrowsOnSetPositionError();

    return 0;
}
