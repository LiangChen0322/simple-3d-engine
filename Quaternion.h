
#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vec.h"
#include "Mat.h"
#include <iostream>

namespace g3 {

struct Quaternion {
  Vec3 v;
  float s;
}; // struct Quaternion

Quaternion createQuaternion(const Vec3& axis, const float rad);
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

float magnitude(const Quaternion& q);
Quaternion normalize(const Quaternion& q);

Mat4 createRotationMatrix(const Quaternion& q);

std::ostream& operator<<(std::ostream& out, const Quaternion& q);

} // namespace g3

#endif // QUATERNION_H

