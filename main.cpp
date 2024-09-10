#include <iostream>
#include <stdexcept>
#include <functional>
#include "spaceship.h"
#include "movement.h"
#include "exception_queue.h"

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

int main() {
    testMoveChangesPositionCorrectly();
    testMoveThrowsOnPositionReadError();
    testMoveThrowsOnVelocityReadError();
    testMoveThrowsOnSetPositionError();

    testLogCommandLogsException();           // Test 4
    testHandleExceptionAddsLogCommandToQueue(); // Test 5
    testRetryCommandRetriesExecution();      // Test 6
    testHandleExceptionAddsRetryCommandToQueue(); // Test 7
    testRetryThenLogOnException();           // Test 8
    testRetryTwiceThenLogOnException();      // Test 9

    return 0;
}
