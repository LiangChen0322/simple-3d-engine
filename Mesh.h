
#ifndef MESH_H
#define MESH_H

#include <cstddef>
#include "Vec.h"
#include "Mat.h"
#include "Quaternion.h"
#include <memory>

#define PI  (3.1415926535898 / 180)

namespace g3 {

struct ColorRGBA {
  int r, g, b, a;
};

/**
 * The information we store at the vertex level.
 */
struct Vertex {
  Vec3 pos;
  ColorRGBA color;
};

struct Triangle {
  unsigned int vertexIndex[3];
  Vec3 normal;
}; // struct Triangle

/**
 * Stores an indexed triangle mesh with N vertices and M triangles.
 */
#if 0
struct TriangleMesh {
  unsigned int nVertices;
  std::unique_ptr<Vertex[]> vertices;

  unsigned int nFaces;
  std::unique_ptr<Triangle[]> faces;

  float rotationX, rotationY, rotationZ;

  Vec3 loc;
}; // struct TriangleMesh
#else
struct TriangleMesh {
  Vec3 center;
  Vec3 loc;
  double R;

  unsigned int nVertices;
  std::unique_ptr<Vertex[]> vertices;

  unsigned int nFaces;
  std::unique_ptr<Triangle[]> faces;

  float rotationX, rotationY, rotationZ;
};
#endif

void loadCube(TriangleMesh& mesh);
Mat4 getWorldMatrix(TriangleMesh& mesh);

} // namespace g3

#endif // MESH_H
