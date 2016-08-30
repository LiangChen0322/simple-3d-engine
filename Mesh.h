
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
  /**
   * 3D position of the vertex.
   */
  Vec3 pos;
}; // struct Vertex


/**
 * The information we store at the triangle level.
 */
struct Triangle
{
  unsigned int vertexIndex[3];
}; // struct Triangle


/**
 * Stores an indexed triangle mesh with N vertices and M triangles.
 */
struct TriangleMesh
{
  /**
   * The number of vertices.
   */
  unsigned int nVertices;

  /**
   * The vertex list.
   */
  std::unique_ptr<Vertex[]> vertices;

  /**
   * The number of triangles.
   */
  unsigned int nFaces;

  /**
   * The triangle list.
   */
  std::unique_ptr<Triangle[]> faces;

  /**
   * The rotation angle around the x, y or z axis in radians.
   */
  float rotationX, rotationY, rotationZ;

  /**
   * The position of the mesh in the world.
   */
  Vec3 loc;
}; // struct TriangleMesh


/**
 * Loads a cube triangle mesh.
 */
void loadCube(TriangleMesh& mesh);


/**
 * Calculates the world transformation matrix of the triangle mesh object.
 */
Mat4 getWorldMatrix(TriangleMesh& mesh);


} // namespace g3

#endif // MESH_H
