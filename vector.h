#pragma once
#include <iostream>

class Vector {
public:
    double X;
    double Y;

    Vector(double x = 0, double y = 0) : X(x), Y(y) {}

    Vector operator+(const Vector& vec) const {
        return Vector(X + vec.X, Y + vec.Y);
    }

    bool operator==(const Vector& vec) const {
        return X == vec.X && Y == vec.Y;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
        os << "Vec(" << vec.X << ", " << vec.Y << ")";
        return os;
    }
};
