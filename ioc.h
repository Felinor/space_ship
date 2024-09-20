#pragma once
#include <iostream>
#include <unordered_map>
#include <functional>
#include <memory>
#include <stack>
#include <thread>
#include <stdexcept>
#include <cassert>
#include <vector>
#include <mutex>

class IoC {
public:
    // Метод для регистрации зависимостей
    template <typename T>
    void Register(const std::string& key, std::function<T*(std::vector<void*>)> factory) {
        std::lock_guard<std::mutex> lock(mutex);  // Потокобезопасность
        currentScope()[key] = [factory](std::vector<void*> args) {
            return static_cast<void*>(factory(args));
        };
    }

    // Метод для разрешения зависимостей
    template <typename T>
    T* Resolve(const std::string& key, std::vector<void*> args = {}) {
        std::lock_guard<std::mutex> lock(mutex);
        auto scope = currentScope();
        if (scope.find(key) != scope.end()) {
            return static_cast<T*>(scope[key](args));
        }
        throw std::runtime_error("Dependency not found: " + key);
    }

    // Создание нового скоупа
    void CreateScope(const std::string& scopeId) {
        std::lock_guard<std::mutex> lock(mutex);
        scopesStack[std::this_thread::get_id()].push(std::make_shared<Scope>());
    }

    // Переход в указанный скоуп
    void SwitchScope(const std::string& scopeId) {
        std::lock_guard<std::mutex> lock(mutex);
        if (scopes.find(scopeId) != scopes.end()) {
            scopesStack[std::this_thread::get_id()].push(scopes[scopeId]);
        } else {
            throw std::runtime_error("Scope not found: " + scopeId);
        }
    }

    // Возвращение к предыдущему скоупу
    void RestoreScope() {
        std::lock_guard<std::mutex> lock(mutex);
        scopesStack[std::this_thread::get_id()].pop();
    }

private:
    // Структура для хранения зависимостей в скоупах
    using Scope = std::unordered_map<std::string, std::function<void*(std::vector<void*>)>>;
    std::unordered_map<std::string, std::shared_ptr<Scope>> scopes;
    std::unordered_map<std::thread::id, std::stack<std::shared_ptr<Scope>>> scopesStack;
    std::mutex mutex;  // Для потокобезопасного доступа

    // Возвращает текущий активный скоуп
    Scope& currentScope() {
        if (scopesStack[std::this_thread::get_id()].empty()) {
            scopesStack[std::this_thread::get_id()].push(std::make_shared<Scope>());
        }
        return *scopesStack[std::this_thread::get_id()].top();
    }
};
