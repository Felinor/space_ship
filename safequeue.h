#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>

class SafeQueue {
private:
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> hardStopFlag{false};
    std::atomic<bool> softStopFlag{false};
    std::thread workerThread;

public:
    SafeQueue() = default;

    ~SafeQueue() {
        //        hardStopFlag = true;
        cv.notify_all();
        workerThread.join();
    }

    // Метод добавления задачи
    void addTask(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.emplace(task);
        cv.notify_all();  // Уведомляем поток о новой задаче
    }

    // Старт работы в новом потоке
    void start() {
        workerThread = std::thread(&SafeQueue::processTasks, this);
    }

    // Метод для жесткой остановки
    void hardStop() {
        hardStopFlag = true;
        cv.notify_all();
    }

    // Метод для мягкой остановки
    void softStop() {
        std::cerr << "Soft stop initiated. Exiting after completing all tasks.\n";
        softStopFlag = true;
        cv.notify_all();
    }

private:
    // Основной метод обработки задач
    void processTasks() {
        while (true) {
            std::function<void()> task;

            std::unique_lock<std::mutex> lock(queueMutex);

            if (!tasks.empty()) {
                std::cerr << "Working...\n";
                task = std::move(tasks.front());
                tasks.pop();

                if (task) {
                    try {
                        task();  // Выполнение задачи
                    } catch (const std::exception& e) {
                        std::cerr << "Exception caught during task execution: " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "Unknown exception caught during task execution." << std::endl;
                    }
                }

            } else {
                std::cerr << "Waiting for new tasks... \n";
                cv.wait(lock, [this] { return !tasks.empty(); });
                std::cerr << "...finished waiting.\n";
            }

            if (hardStopFlag && !tasks.empty()) {
                std::cerr << "Hard stop initiated.\n";
                return;  // Завершаем работу, если установлен флаг жесткой остановки
            }

            if (softStopFlag && tasks.empty()) {
                std::cerr << "All tasks are completed after a soft stop.\n";
                return;  // Завершаем работу после выполнения всех задач
            }
        }
    }
};

#endif  // SAFEQUEUE_H
