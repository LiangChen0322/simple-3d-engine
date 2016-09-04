
#include "Mat.h"
#include <cmath>

g3::Mat4 g3::transponse(const Mat4& mat)
{
	return {
		mat[0], mat[4], mat[8],  mat[12],
		mat[1], mat[5], mat[9],  mat[13],
		mat[2], mat[6], mat[10], mat[14],
		mat[3], mat[7], mat[11], mat[15]
	};
	// can I make better performance?
}

g3::Vec3 g3::transformP3(const Vec3& vec, const Mat4& mat)
{
	float x =  (vec[0] * mat[0]) + (vec[1] * mat[4]) + (vec[2] * mat[8])  + mat[12];
	float y = (vec[0] * mat[1]) + (vec[1] * mat[5]) + (vec[2] * mat[9])  + mat[13];
	float z =  (vec[0] * mat[2]) + (vec[1] * mat[6]) + (vec[2] * mat[10]) + mat[14];
	float w = (vec[0] * mat[3]) + (vec[1] * mat[7]) + (vec[2] * mat[11]) + mat[15];	
	
	if (w != 1 && w != 0) {
		x /= w;
		y /= w;
		z /= w;
	}

	return Vec3 { x, y, z };
}

/**
 * Transforms a 3D vector with a 4x4 matrix.
 */
g3::Vec3 g3::transformV3(const Vec3& vec, const Mat4& mat)
{
	return Vec3 {
		(vec[0] * mat[0]) + (vec[1] * mat[4]) + (vec[2] * mat[8]),
		(vec[0] * mat[1]) + (vec[1] * mat[5]) + (vec[2] * mat[9]),
		(vec[0] * mat[2]) + (vec[1] * mat[6]) + (vec[2] * mat[10])
	};
}

g3::Mat4 g3::createScaleMatrix(const float scale)
{
	return Mat4 {
		scale, 0,     0,     0,
		0,     scale, 0,     0,
		0,     0,     scale, 0,
		0,     0,     0,     1
	};
}

g3::Mat4 g3::createScaleMatrix(const float scaleX, const float scaleY, const float scaleZ)
{
	return Mat4 {
		scaleX, 0,      0,      0,
		0,      scaleY, 0,      0,
		0,      0,      scaleZ, 0,
		0,      0,      0,      1
	};
}

g3::Mat4 g3::createTranslationMatrix(const float valX, const float valY, const float valZ)
{
	return Mat4 {
		1,    0,    0,    0,
		0,    1,    0,    0,
		0,    0,    1,    0,
		valX, valY, valZ, 1
	};
}

g3::Mat4 g3::createRotationXMatrix(const float rad)
{
	return Mat4 {
		1, 0,             0,              0,
		0, std::cos(rad), std::sin(rad), 0,
		0, -std::sin(rad), std::cos(rad),  0,
		0, 0,             0,              1
	};
}

g3::Mat4 g3::createRotationYMatrix(const float rad)
{
	return Mat4 {
		std::cos(rad),  0, -std::sin(rad), 0,
		0,              1, 0,             0,
		std::sin(rad), 0, std::cos(rad), 0,
		0,              0, 0,             1
	};
}

g3::Mat4 g3::createRotationZMatrix(const float rad)
{
	return Mat4 {
		std::cos(rad), std::sin(rad), 0, 0,
		-std::sin(rad), std::cos(rad),  0, 0,
		0,             0,              1, 0,
		0,             0,              0, 1
	};
}

g3::Mat4 g3::createLookAtLHMatrix(const Vec3& eye, const Vec3& target, const Vec3& up)
{
	Vec3 F = g3::normalize(target - eye);
	Vec3 L = g3::normalize(g3::crossProduct(up, F));
	Vec3 U = crossProduct(F, L);

	return Mat4 {
		L[0],                U[0],                F[0],                0,
		L[1],                U[1],                F[1],                0,
		L[2],                U[2],                F[2],                0,
		-dotProduct(L, eye), -dotProduct(U, eye), -dotProduct(F, eye), 1
	};
}

g3::Mat4 g3::createPerspectiveFovLHMatrix(float fieldOfViewY, float aspectRatio, float zNearPlane, float zFarPlane)
{
	// cot(x) = tan(PI/2 - x)
	float h = std::tan((g3::PI/2) - (fieldOfViewY/2));
	float w = h * aspectRatio;

	return Mat4 {
		w, 0, 0,                                                  0,
		0, h, 0,                                                  0,
		0, 0, zFarPlane / (zFarPlane - zNearPlane) ,              1,
		0, 0, (-zNearPlane*zFarPlane) / (zFarPlane - zNearPlane), 0
	};
}
