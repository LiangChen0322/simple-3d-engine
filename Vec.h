
#ifndef VEC_H
#define VEC_H

#include <cstddef>
#include <memory>
#include <cmath>
#include <iostream>

namespace g3
{

template<std::size_t N>
class Vec
{
public:

  Vec():mScalars{} {}

  Vec(std::initializer_list<float> scalars): mScalars() {
    std::size_t i = 0;
    for (float f : scalars) {
      if (N == i) break;
      mScalars[i++] = f;
    }
  }

  Vec(const Vec<N>& other): mScalars{} {
  	std::copy(other.mScalars, other.mScalars + N, mScalars);
  }

  Vec(Vec<N>&& other): mScalars{} {
   swap(*this, other);
  }

  Vec<N>& operator=(Vec<N> other) {
    swap(*this, other);
    return *this;
  }

  float& operator[] (const int ind) { return mScalars[ind]; }
  const float& operator[] (const int ind) const { return mScalars[ind]; }

  Vec<N> operator-() const {
    g3::Vec<N> res;
    for (int i = 0; i < N; i++) {
      res[i] = -((*this)[i]);
    }
    return res;
  }

  Vec<N> operator+(const Vec<N>& rhs) const {
    g3::Vec<N> res;
    for (int i = 0; i < N; i++) {
      res[i] = (*this)[i] + rhs[i];
    }
    return res;
  }

  Vec<N> operator-(const Vec<N>& rhs) const {
    return (*this) + -rhs;
  }

  Vec<N> operator*(const float scalar) const {
    g3::Vec<N> res;
    for (int i = 0; i < N; i++) {
      res[i] = (*this)[i] * scalar;
    }
    return res;
  }

  float length() const { 
    float res = 0;
    for (int i = 0; i < N; i++) {
      res += (*this)[i] * (*this)[i];
    }
    return std::sqrt(res);
  }

  friend void swap(Vec<N>& first, Vec<N>& second) {
    std::swap(first.mScalars, second.mScalars);
  }


 private:
  float mScalars[N];
};

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;
using Vec4 = Vec<4>;

template<std::size_t N>
Vec<N> operator*(const float scalar, const Vec<N>& vec)
{
  return vec * scalar;
}

template<std::size_t N>
Vec<N> normalize(const Vec<N>& vec)
{
  Vec<N> res;
  float factor = N > 0 ? 1 / vec.length() : 1;
  for (int i = 0; i < N; i++) {
    res[i] = vec[i] * factor;
  }
  return res;
}

template<std::size_t N>
float dotProduct(const Vec<N>& lhs, const Vec<N>& rhs)
{
  float res = 0;
  for (int i = 0; i < N; i++) {
    res += lhs[i] * rhs[i];
  }
  return res;
}

/*template<std::size_t N>
int operator==(const Vec<N>& lhs, const Vec<N> &ths)
{
  for (int i = 0; i < N; i++) {
    if (lhs[i] != rhs[i])
      return 0;
  }
  return 1;
}*/

Vec3 crossProduct(const Vec3&, const Vec3&);

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const Vec<N>& vec)
{
  for (int i = 0; i < N; i++) {
    if (i > 0) {
      out << ", ";
    }
    out << vec[i];
  }

  return out;
}

}
#endif
