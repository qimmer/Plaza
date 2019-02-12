//
// Created by Kim on 06/07/2018.
//

#ifndef PLAZA_MATH_H
#define PLAZA_MATH_H

#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Clamp(x, low, high) Max(Min(x, high), low)

#define Abs(x) (((x) > 0) ? x : -x)

inline unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
{
    return (((((iR << 8) + iG) << 8) + iB) << 8) + iA;
}

inline unsigned long UpperPowerOf2(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline int Align(int number, int multiple)
{
    if (multiple == 0)
        return number;

    int remainder = Abs(number) % multiple;
    if (remainder == 0)
        return number;

    if (number < 0)
        return -(Abs(number) - remainder);
    else
        return number + multiple - remainder;
}

#endif //PLAZA_MATH_H
