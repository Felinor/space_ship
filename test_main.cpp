#include <gtest/gtest.h>

#include "spaceship.h"
#include "movement.h"
#include "rotation.h"

TEST(MovementTests, MoveChangesPositionCorrectly) {
    SpaceShip ship(Vector(12, 5), 0.0);  // Создаем корабль в точке (12, 5)
    ship.setVelocity(Vector(-7, 3));     // Устанавливаем скорость (-7, 3)

    Movement::Move(ship);  // Перемещаем объект

    EXPECT_EQ(ship.getPosition(), Vector(5, 8));  // Проверяем, что новое положение (5, 8)
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
