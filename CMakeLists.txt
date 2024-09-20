# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(SpaceshipProject)

# Версия стандарта C++
set(CMAKE_CXX_STANDARD 17)

# Флаги компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Добавление исходных файлов
add_executable(spaceship main.cpp
                         exception_queue.h
                         movable.h
                         movement.h
                         rotation.h
                         spaceship.h
                         vector.h
                         checkFuelCommand.h
                         burnFuelCommand.h
                         macroCommand.h
                         moveWithFuel.h
                         changeVelocity.h
                         rotateAndChangeVelocity.h
                         ioc.h)

# Подключение Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
# Устанавливаем, чтобы не устанавливать gtest в систему, а использовать его локально
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Включаем тестирование
enable_testing()

# Создание цели для тестов
add_executable(tests test_main.cpp)

# Линкуем Google Test с тестами
target_link_libraries(tests gtest gtest_main)

# Добавляем тесты
add_test(NAME UnitTests COMMAND tests)
