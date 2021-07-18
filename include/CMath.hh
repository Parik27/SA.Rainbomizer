#pragma once

template <typename T> struct Vector2_t
{
    T x;
    T y;
};

template <typename T> struct Vector3_t : public Vector2_t<T>
{
    T z;

    inline Vector3_t<T> &
    operator+= (const Vector3_t<T> &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;

        return *this;
    }

    inline friend Vector3_t<T>
    operator+ (Vector3_t<T> lhs, const Vector3_t<T> &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    inline Vector3_t<T> &
    operator-= (const Vector3_t<T> &rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;

        return *this;
    }

    inline friend Vector3_t<T>
    operator- (Vector3_t<T> lhs, const Vector3_t<T> &rhs)
    {
        lhs -= rhs;
        return lhs;
    }
};

typedef Vector3_t<float> Vector3;
