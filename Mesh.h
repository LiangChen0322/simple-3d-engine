
#ifndef MESH_H
#define MESH_H

#include <cstddef>
#include "Vec.h"
#include "Mat.h"
#include "Quaternion.h"
#include <memory>

namespace g3 
{

/**
 * The information we store at the vertex level.
 */
struct Vertex
{
  Vec3 pos;
};

struct Triangle
{
  unsigned int vertexIndex[3];
}; // struct Triangle


/**
 * Stores an indexed triangle mesh with N vertices and M triangles.
 */
struct TriangleMesh
{
  unsigned int nVertices;
  std::unique_ptr<Vertex[]> vertices;

  unsigned int nFaces;
  std::unique_ptr<Triangle[]> faces;

  float rotationX, rotationY, rotationZ;

  Vec3 loc;
}; // struct TriangleMesh

void loadCube(TriangleMesh& mesh);
Mat4 getWorldMatrix(TriangleMesh& mesh);


} // namespace g3

#endif // MESH_H
