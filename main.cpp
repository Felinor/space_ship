#include <iostream>
#include <stdexcept>
#include <functional>
#include "spaceship.h"
#include "movement.h"
#include "exception_queue.h"
#include "checkFuelCommand.h"
#include "burnFuelCommand.h"
#include "macroCommand.h"
#include "rotateAndChangeVelocity.h"
#include "moveWithFuel.h"
#include "changeVelocity.h"

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

// Тесты

void testLogCommandLogsException() {
    CommandQueue queue;
    std::cout << "\nTest 4: LogCommand logs exception\n";
    try {
        auto failingCommand = std::make_shared<FailingCommand>();
        throw std::runtime_error("Test exception");
    } catch (const std::exception& ex) {
        auto cmd = std::make_shared<FailingCommand>();
        auto logCmd = std::make_shared<LogCommand>(cmd, ex);
        queue.AddCommand(logCmd);
    }
    queue.ProcessCommands();  // Ожидаем вывод логирования исключения
}

void testHandleExceptionAddsLogCommandToQueue() {
    CommandQueue queue;
    std::cout << "\nTest 5: HandleException adds LogCommand to queue\n";
    auto failingCommand5 = std::make_shared<FailingCommand>();
    try {
        failingCommand5->Execute();
    } catch (const std::exception& ex) {
        queue.HandleException(failingCommand5, ex);
    }
    queue.ProcessCommands();  // Ожидаем добавления LogCommand
}

void testRetryCommandRetriesExecution() {
    CommandQueue queue;
    std::cout << "\nTest 6: RetryCommand retries execution\n";
    auto retryCmd = std::make_shared<RetryCommand>(std::make_shared<FailingCommand>(), 2);
    queue.AddCommand(retryCmd);
    queue.ProcessCommands();  // Ожидаем повтор выполнения команды
}

void testHandleExceptionAddsRetryCommandToQueue() {
    CommandQueue queue;
    std::cout << "\nTest 7: HandleException adds RetryCommand to queue\n";
    auto failingCommand7 = std::make_shared<FailingCommand>();
    try {
        failingCommand7->Execute();
    } catch (const std::exception& ex) {
        queue.HandleException(failingCommand7, ex);
    }
    queue.ProcessCommands();  // Ожидаем добавления RetryCommand
}

void testRetryThenLogOnException() {
    CommandQueue queue;
    std::cout << "\nTest 8: Retry then Log on exception\n";
    auto failingCommand8 = std::make_shared<FailingCommand>();
    queue.AddCommand(failingCommand8);
    queue.ProcessCommands();  // Ожидаем повтор и логирование после второго исключения
}

void testRetryTwiceThenLogOnException() {
    CommandQueue queue;
    std::cout << "\nTest 9: Retry twice, then log on exception\n";
    auto retryTwiceCmd = std::make_shared<RetryTwiceCommand>(std::make_shared<FailingCommand>());
    queue.AddCommand(retryTwiceCmd);
    queue.ProcessCommands();  // Ожидаем два повтора и логирование ошибки после третьего исключения
}

// Тест проверяет достаточно ли топлива для выполнения действия
void testCheckFuelCommandPasses() {
    SpaceShip ship(Vector(0, 0), 0);
    ship.setFuel(100);
    CheckFuelCommand checkFuelCmd(ship, 50);
    checkFuelCmd.Execute();  // Должно пройти без исключений
    std::cout << "Test CheckFuelCommandPasses passed." << std::endl;
}

// Если топлива недостаточно - получаем исключение
void testCheckFuelCommandFails() {
    SpaceShip ship(Vector(0, 0), 0);
    ship.setFuel(30);
    CheckFuelCommand checkFuelCmd(ship, 50);
    assertThrows([&]() { checkFuelCmd.Execute(); }, "CheckFuelCommandFails");
}

// Тест уменьшения количества топлива при выполнении команды
void testBurnFuelCommand() {
    SpaceShip ship(Vector(0, 0), 0);
    ship.setFuel(100);
    BurnFuelCommand burnFuelCmd(ship, 20);
    burnFuelCmd.Execute();  // Сжигаем 20 единиц топлива
    assertEquals(ship.getFuel(), 80, "BurnFuelCommand");
}

// Тест будет выполнять несколько команд последовательно, останавливая выполнение при выбросе исключения
void testMoveWithFuelConsumption() {
    SpaceShip ship(Vector(0, 0), 0.0);
    ship.setFuel(10);  // Устанавливаем топливо

    double fuelConsumptionRate = 5; // Скорость расхода топлива

    // Создаем цепочку команд
    std::vector<std::shared_ptr<Command>> commands = {
        std::make_shared<CheckFuelCommand>(ship, fuelConsumptionRate),
        std::make_shared<MoveWithFuelCommand>(ship, fuelConsumptionRate),
        std::make_shared<BurnFuelCommand>(ship, fuelConsumptionRate)
    };

    MacroCommand moveWithFuel(commands);
    moveWithFuel.Execute();  // Выполняем макрокоманду
}

// Тест изменения вектора скорости
void testChangeVelocityCommand() {
    SpaceShip ship(Vector(0, 0), 0);
    Vector newVelocity(10, 5);
    ChangeVelocityCommand changeVelocity(ship, newVelocity);

    changeVelocity.Execute();
    if (ship.getVelocity() == newVelocity) {
        std::cout << "Test ChangeVelocityCommand passed.\n";
    } else {
        std::cerr << "Test ChangeVelocityCommand failed.\n";
    }
}

// Тест команды поворота, которая еще и меняет вектор мгновенной скорости
void testRotateAndChangeVelocity() {
    SpaceShip ship(Vector(0, 0), 0);
    ship.setFuel(10);

    double fuelConsumptionRate = 5;

    // Создаем цепочку команд
    std::vector<std::shared_ptr<Command>> commands = {
        std::make_shared<CheckFuelCommand>(ship, fuelConsumptionRate),
        std::make_shared<MoveWithFuelCommand>(ship, fuelConsumptionRate),
        std::make_shared<ChangeVelocityCommand>(ship, Vector(10, 10)),
        std::make_shared<RotateAndChangeVelocity>(ship, 90, Vector(5, 5)),
        std::make_shared<BurnFuelCommand>(ship, fuelConsumptionRate)
    };

    MacroCommand rotateAndChangeVelocity(commands);
    rotateAndChangeVelocity.Execute();

    // Проверяем, что корабль повернулся на 90 градусов
    assertEquals(ship.getRotation(), 90.0, "RotateAndChangeVelocity: Rotation");

    // Проверяем, что скорость изменена
    assertEquals(ship.getVelocity(), Vector(-10, 10), "RotateAndChangeVelocity: Velocity");
}

// Тест команды поворота, которая не меняет вектор мгновенной скорости
void testRotateAndWithoutChangeVelocity() {
    SpaceShip ship(Vector(0, 0), 0);

    Vector velocity(0, 0);

    // Создаем цепочку команд
    std::vector<std::shared_ptr<Command>> noMovementCommands = {
        std::make_shared<ChangeVelocityCommand>(ship, velocity),
        std::make_shared<RotateAndChangeVelocity>(ship, 45, velocity),
    };

    MacroCommand noMovementRotate(noMovementCommands);
    noMovementRotate.Execute();

    // Проверяем, что корабль повернулся на 45 градусов
    assertEquals(ship.getRotation(), 45.0, "RotateWithoutChangeVelocity: Rotation with no movement");
    // Проверяем, что скорость осталась нулевой
    assertEquals(ship.getVelocity(), velocity, "RotateWithoutChangeVelocity: Velocity with no movement");
}

int main() {
    testCheckFuelCommandPasses();
    testCheckFuelCommandFails();
    testBurnFuelCommand();
    testMoveWithFuelConsumption();
    testChangeVelocityCommand();
    testRotateAndChangeVelocity();
    testRotateAndWithoutChangeVelocity();

//    testMoveChangesPositionCorrectly();
//    testMoveThrowsOnPositionReadError();
//    testMoveThrowsOnVelocityReadError();
//    testMoveThrowsOnSetPositionError();

//    testLogCommandLogsException();           // Test 4
//    testHandleExceptionAddsLogCommandToQueue(); // Test 5
//    testRetryCommandRetriesExecution();      // Test 6
//    testHandleExceptionAddsRetryCommandToQueue(); // Test 7
//    testRetryThenLogOnException();           // Test 8
//    testRetryTwiceThenLogOnException();      // Test 9

    return 0;
}
