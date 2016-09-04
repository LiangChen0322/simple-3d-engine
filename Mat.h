
#ifndef MAT_H
#define MAT_H

#include <cstddef>
#include <memory>
#include <initializer_list>
#include "Vec.h"
#include <iostream>

namespace g3
{

/**
 * Represents a NxN square matrix.
 */
template<std::size_t N>
class Mat
{
  public:
  /**
   * Default constructor.
   */
  Mat():mScalars{} {}

  Mat(std::initializer_list<float> values): mScalars() {
  std::size_t i = 0, j = 0;
    for (float f : values) {
      if (i == N*N) break;
      mScalars[i++] = f;
    }
  }

  /**
  * Copy constructor
  */
  Mat(const Mat<N>& other): mScalars{} {
    std::copy(other.mScalars, other.mScalars + (N*N), mScalars);
  }

  /**
  * Move constructor
  */
  Mat(Mat<N>&& other): mScalars{} {
    swap(*this, other);
  }

  /**
  * Copy assignment operator 
  */
  Mat<N>& operator=(Mat<N> other) {
    swap(*this, other);
    return *this;
  }

  /**
  * Subscript operator overloading
  */
  float& operator[](const std::size_t ind) { return mScalars[ind]; }
  const float& operator[](const std::size_t ind) const { return mScalars[ind]; }

	/**
	 * Matrix multiplication operator overloading
	 */
  Mat<N> operator*(const Mat<N>& other) const
  {
    Mat<N> res;
    for (std::size_t i = 0; i<N; i++) {
      for (std::size_t j = 0, k = 0; (j<N) || (k<N); ) {
        res[i*N+k] += (*this)[i*N+j] * other[j*N+k]; 

        if ((++j == N) && (++k != N)) { j=0; }
      }
    }
    return res;
  }

	/**
	 * Scalar multiplication operator overloading
	 */
  Mat<N> operator*(const float scalar) const
  {
    Mat<N> res;
    for (std::size_t i = 0; i<N; i++) {
      for (std::size_t j = 0; j<N; j++) {
        res[i*N+j] = (*this)[i*N+j] * scalar;
      }
    }
    return res;
  }

  /**
  * Swaps two matrices. Used by the assignment operators (copy, move).
  */
  friend void swap(Mat<N>& first, Mat<N>& second) {
    std::swap(first.mScalars, second.mScalars);
  }

  private:

  /**
   * A float array which contains the elements of the matrix.
   */
  float mScalars[N*N];
};

/**
 * Scalar multiplication operator overloading
 */
template<std::size_t N>
Mat<N> operator*(const float scalar, const Mat<N>& mat) {
  // avoid code duplication
  return mat * scalar;
}

/**
 * Replaces a matrix with the identity matrix.
 */
template<std::size_t N>
Mat<N>& loadIdentity(Mat<N>& mat) {
  std::size_t ind = 0;
  for (std::size_t i = 0; i<N; i++) {
    for (std::size_t j = 0; j<N; j++) {
      ind = i*N+j;
      mat[ind] = (ind % (N+1) == 0) ? 1 : 0;
    }
  }
  return mat;
}

using Mat2 = Mat<2>;
using Mat3 = Mat<3>;
using Mat4 = Mat<4>;

/**
 * Transforms a 3D point with a 4x4 matrix.
 */
Vec3 transformP3(const Vec3& vec, const Mat4& mat);
Vec3 transformV3(const Vec3& vec, const Mat4& mat);

Mat4 createScaleMatrix(const float scale);
Mat4 createScaleMatrix(const float scaleX, const float scaleY, const float scaleZ);
Mat4 createTranslationMatrix(const float valX, const float valY, const float valZ);

/**
 * PI constant
 */
const float PI = 3.14159265358979;
constexpr float toRad(const float deg)
{
  return deg * PI / 180.0; 
}

constexpr float toDeg(const float rad)
{
  return rad * 180.0 / PI;
}

/* Transponses a matrix. */
Mat4 transponse(const Mat4& mat);

/**
 * Returns a rotation matrix in row major order that can be used to rotate 
 * a set of vertices around the x-axis.
 */
Mat4 createRotationXMatrix(const float rad);
Mat4 createRotationYMatrix(const float rad);
Mat4 createRotationZMatrix(const float rad);

/**
 * Creates a left-handed look-at matrix in row major order.
 */
Mat4 createLookAtLHMatrix(const Vec3& eye, const Vec3& target, const Vec3& up);

/**
 * Creates a left-handed perspective projection matrix based on a field of view.
 */
Mat4 createPerspectiveFovLHMatrix(float fieldOfViewY, float aspectRatio, float zNearPlane, float zFarPlane);

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const Mat<N>& mat)
{
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      out << mat[i*N+j] << ", ";
    }

    if (i < N-1) {
      out << std::endl;
    }
  }

  return out;
}

}

#endif
