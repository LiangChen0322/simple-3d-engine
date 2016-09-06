
#include "Mesh.h"
#include "Vec.h"
#include "Quaternion.h"
#include "Mat.h"

/**
 * Loads a cube triangle mesh.
 */

// void g3::loadCube(g3::TriangleMesh& mesh)
// {
//   mesh.nVertices = 4;
//   mesh.vertices.reset(new Vertex[mesh.nVertices]);

//   // float vs[] {1,0,0, -1,0,0, 0,0,-1.6, 0,2,-1};
//   float vs[] {0,1,0, 2,0,0, 0,2,0, 0,0,2};
//   int color[] {255,0,0,255, 0,255,0,255, 255,255,255,255, 0,0,255,255};

//   for (unsigned int i = 0, j = 0; i < mesh.nVertices; i++, j+=3) {
//     mesh.vertices[i].pos = { vs[j], vs[j+1], vs[j+2] };
//   }
//   for (unsigned int i = 0, j = 0; i < mesh.nVertices; i++, j+=4) {
//     mesh.vertices[i].color = { color[j], color[j+1], color[j+2], color[j+3]};
//   }

//   mesh.nFaces = 4;
//   mesh.faces.reset(new Triangle[mesh.nFaces]);

//   unsigned int indices[] {0,1,2, 0,2,3, 0,1,3, 1,2,3};
//   for  (unsigned int i = 0, j = 0; i < mesh.nFaces; i++, j+=3) {
//     mesh.faces[i].vertexIndex[0] = indices[j];
//     mesh.faces[i].vertexIndex[1] = indices[j+1];
//     mesh.faces[i].vertexIndex[2] = indices[j+2];
//   }

//   mesh.rotationX = mesh.rotationY = mesh.rotationZ = 0;
//   mesh.loc = {1, 1, 1};
//   // mesh.loc = {4, 2, -2};
// }

void g3::loadCube(g3::TriangleMesh& mesh)
{
  mesh.nVertices = 3;
  mesh.vertices.reset(new Vertex[mesh.nVertices]);

  // float vs[] {1,0,0, -1,0,0, 0,0,-1.6, 0,2,-1};
  float vs[] {2,0,0, 0,2,0, 0,0,2};
  int color[] {255,0,0,255, 0,255,0,255, 0,0,255,255};

  for (unsigned int i = 0, j = 0; i < mesh.nVertices; i++, j+=3) {
    mesh.vertices[i].pos = { vs[j], vs[j+1], vs[j+2] };
  }
  for (unsigned int i = 0, j = 0; i < mesh.nVertices; i++, j+=4) {
    mesh.vertices[i].color = { color[j], color[j+1], color[j+2], color[j+3]};
  }

  mesh.nFaces = 1;
  mesh.faces.reset(new Triangle[mesh.nFaces]);

  unsigned int indices[] {0,1,2};
  for  (unsigned int i = 0, j = 0; i < mesh.nFaces; i++, j+=3) {
    mesh.faces[i].vertexIndex[0] = indices[j];
    mesh.faces[i].vertexIndex[1] = indices[j+1];
    mesh.faces[i].vertexIndex[2] = indices[j+2];
  }

  mesh.rotationX = mesh.rotationY = mesh.rotationZ = 0;
  mesh.loc = {0, 0, 0};
  // mesh.loc = {4, 2, -2};
}
/**
 * Calculates the world transformation matrix of the triangle mesh object.
 */
g3::Mat4 g3::getWorldMatrix(g3::TriangleMesh& mesh)
{
  Vec3 axisX {1, 0, 0};
  Vec3 axisY {0, 1, 0};
  Vec3 axisZ {0, 0, 1};
  Quaternion rotX = g3::createQuaternion(axisX, mesh.rotationX);
  Quaternion rotY = g3::createQuaternion(axisY, mesh.rotationY);
  Quaternion rotZ = g3::createQuaternion(axisZ, mesh.rotationZ);

  return g3::createRotationMatrix(rotZ * rotY * rotX) * g3::createTranslationMatrix(mesh.loc[0], mesh.loc[1], mesh.loc[2]);
}
