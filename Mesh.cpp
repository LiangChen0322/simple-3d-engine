
#include "Mesh.h"
#include "Vec.h"
#include "Quaternion.h"
#include "Mat.h"
#include <stdio.h>

/**
 * Loads a cube triangle mesh.
 */
void g3::loadCube(g3::TriangleMesh& mesh)
{
  FILE *pf;

  pf = fopen("./model/model.txt", "r");
  if (pf == NULL) return;

  fscanf(pf, "%d", &mesh.nVertices);
  mesh.vertices.reset(new Vertex[mesh.nVertices]);
  for (int i = 0; i < mesh.nVertices; i++) {
    float vs[3];
    fscanf(pf, "%f %f %f", vs, vs+1, vs+2);
    mesh.vertices[i].pos = { vs[0], vs[1], vs[2] };
  }
  for (int i = 0; i < mesh.nVertices; i++) {
    int rgb[4];
    fscanf(pf, "%d %d %d %d", rgb, rgb+1, rgb+2, rgb+3);
    mesh.vertices[i].color = { rgb[0], rgb[1], rgb[2], rgb[3]};
  }

  fscanf(pf, "%d", &mesh.nFaces);
  mesh.faces.reset(new Triangle[mesh.nFaces]);
  for  (unsigned int i = 0, j = 0; i < mesh.nFaces; i++, j+=3) {
    unsigned int indices[3];
    fscanf(pf, "%u %u %u", indices, indices+1, indices+2);
    mesh.faces[i].vertexIndex[0] = indices[0];
    mesh.faces[i].vertexIndex[1] = indices[1];
    mesh.faces[i].vertexIndex[2] = indices[2];
  }

  fscanf(pf, "%f %f %f", &mesh.rotationX, &mesh.rotationY, &mesh.rotationZ);
  float loc[3];
  fscanf(pf, "%f %f %f", loc, loc+1, loc+2);
  mesh.loc = {loc[0], loc[1], loc[2]};
  fclose(pf);
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
