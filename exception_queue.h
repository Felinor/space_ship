#pragma once
#include <iostream>
#include <queue>
#include <stdexcept>
#include <memory>
#include <typeinfo>

class LogCommand;
class RetryCommand;

// Базовый класс команды
class Command {
public:
    virtual ~Command() = default;
    // Виртуальная функция для выполнения команды
    virtual void Execute() = 0;
    // Виртуальная функция для получения имени команды
    virtual std::string GetName() const = 0;
};

// Очередь команд
class CommandQueue {
private:
    std::queue<std::shared_ptr<Command>> commands;

public:
    void AddCommand(std::shared_ptr<Command> cmd) {
        commands.push(cmd);
    }

    void ProcessCommands() {
        while (!commands.empty()) {
            auto cmd = commands.front();
            commands.pop();

            try {
                cmd->Execute();  // Выполнение команды
            } catch (const std::exception& ex) {
                std::cerr << "Exception caught: " << ex.what() << std::endl;
                HandleException(cmd, ex);  // Обработка исключений
            }
        }
    }

    // Обработчик исключений
    void HandleException(std::shared_ptr<Command> cmd, const std::exception& ex) {
        if (typeid(ex) == typeid(std::runtime_error)) {
            std::cerr << "Handling runtime_error for command: " << cmd->GetName() << std::endl;
            // Добавляем команду-повторитель
            auto retryCmd = std::make_shared<RetryCommand>(cmd);
            AddCommand(std::static_pointer_cast<Command>(retryCmd));
        } else {
            std::cerr << "Logging error for command: " << cmd->GetName() << std::endl;
            // Логируем ошибку
            AddCommand(std::static_pointer_cast<Command>(std::make_shared<LogCommand>(cmd, ex)));
        }
    }
};

// Команда записи в лог
class LogCommand : public Command {
private:
    std::shared_ptr<Command> originalCommand;
    std::string exceptionMessage;

public:
    LogCommand(std::shared_ptr<Command> cmd, const std::exception& ex)
        : originalCommand(cmd), exceptionMessage(ex.what()) {}

    void Execute() override {
        std::cerr << "Logging exception: " << exceptionMessage
                  << " from command: " << originalCommand->GetName() << std::endl;
    }

    std::string GetName() const override {
        return "LogCommand";
    }
};

// Команда повтора с ограничением на количество попыток
class RetryCommand : public Command {
private:
    std::shared_ptr<Command> originalCommand;
    int retryCount;
    int maxRetries;

public:
    RetryCommand(std::shared_ptr<Command> cmd, int maxRetries = 1)
        : originalCommand(cmd), retryCount(0), maxRetries(maxRetries) {}

    void Execute() override {
        if (retryCount < maxRetries) {
            retryCount++;
            std::cerr << "Retrying command: " << originalCommand->GetName() << std::endl;
            try {
                originalCommand->Execute();
            } catch (const std::exception& ex) {
                if (retryCount == maxRetries) {
                    std::cerr << "Max retries reached for command: " << originalCommand->GetName() << std::endl;
                    throw ex;
                } else {
                    throw;  // Бросаем снова для повторного выполнения
                }
            }
        } else {
            throw std::runtime_error("Max retries reached for command: " + originalCommand->GetName());
        }
    }

    std::string GetName() const override {
        return "RetryCommand";
    }
};


// Команда, которая выбрасывает исключение
class FailingCommand : public Command {
private:
    int retryCount;

public:
    FailingCommand() : retryCount(0) {}

    void Execute() override {
        retryCount++;
        if (retryCount < 3) {
            throw std::runtime_error("Failing command error");
        }
        std::cerr << "Command succeeded after retries." << std::endl;
    }

    std::string GetName() const override {
        return "FailingCommand";
    }
};

// Команда, которая дважды повторяет выполнение команды
class RetryTwiceCommand : public Command {
private:
    std::shared_ptr<Command> originalCommand;
    int retryCount;

public:
    RetryTwiceCommand(std::shared_ptr<Command> cmd) : originalCommand(cmd), retryCount(0) {}

    void Execute() override {
        if (retryCount < 2) {
            retryCount++;
            std::cerr << "Retrying command twice: " << originalCommand->GetName() << std::endl;
            originalCommand->Execute();
        } else {
            throw std::runtime_error("Command failed after 2 retries");
        }
    }

    std::string GetName() const override {
        return "RetryTwiceCommand";
    }
};
