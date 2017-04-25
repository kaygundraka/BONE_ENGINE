#include "Function.h"

float GetAngle(const Vec3& a, const Vec3& b)
{
    float cosAngle = D3DXVec3Dot(&a, &b);

    return (a.z*b.x + a.x*b.z > 0.0f) ? (float)(acos(cosAngle)) : -(float)(acos(cosAngle));
}