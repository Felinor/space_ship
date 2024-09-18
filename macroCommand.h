#pragma once
#include "exception_queue.h"
#include <vector>
#include <stdexcept>
#include <memory>

class MacroCommand : public Command {
private:
    std::vector<std::shared_ptr<Command>> commands;

public:
    MacroCommand(const std::vector<std::shared_ptr<Command>>& commands)
            : commands(commands) {}

    void AddCommand(std::shared_ptr<Command> cmd) {
        commands.push_back(cmd);
    }

    void Execute() override {
        for (const auto& cmd : commands) {
            try {
                cmd->Execute();
            } catch (...) {
                throw std::runtime_error("MacroCommand execution stopped due to exception.");
            }
        }
    }

    std::string GetName() const override {
        return "MacroCommand";
    }
};
