
#include "Mesh.h"
#include "Vec.h"
#include "Quaternion.h"
#include "Mat.h"
#include <stdio.h>

void g3::getNormalVector(g3::TriangleMesh& mesh, unsigned int fnum)
{
  // float a, b, c;
  Vec3 p1, p2, p3;
  Vec3 check;

  p1 = mesh.vertices[ mesh.faces[fnum].vertexIndex[1] ].pos - mesh.vertices[ mesh.faces[fnum].vertexIndex[0] ].pos;
  p2 = mesh.vertices[ mesh.faces[fnum].vertexIndex[2] ].pos - mesh.vertices[ mesh.faces[fnum].vertexIndex[0] ].pos;
  p3 = crossProduct(p1, p2);
  check = mesh.vertices[ mesh.faces[fnum].vertexIndex[0] ].pos - mesh.center;

  float ans = dotProduct(p3, check);
  if (ans < 0) {
    mesh.faces[fnum].normal = {-p3[0], -p3[1], -p3[2]};
  } else {
    mesh.faces[fnum].normal = p3;
  }

  // std::cout << "i:" << fnum << "   " << mesh.faces[fnum].normal << std::endl;
}

/**
 * Loads a mesh triangle mesh.
 */
#if 0
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
#else
void g3::loadCube(g3::TriangleMesh& mesh)
{
  float tR, r;
  float x, y, z;

  mesh.loc = {0, 0, 0};
  mesh.R = tR = 2;
  mesh.center = {0, tR, 0};
  mesh.nVertices = 62;
  // mesh.nVertices = 13;
  mesh.vertices.reset(new Vertex[mesh.nVertices]);

  mesh.nFaces = 120;
  // mesh.nFaces = 108;
  mesh.faces.reset(new Triangle[mesh.nFaces]);

  mesh.rotationX = 0.0;
  mesh.rotationY = 0.0;
  mesh.rotationZ = 0.0;

  mesh.vertices[0].pos = {0, tR * 2, 0};
  mesh.vertices[0].color = {0, 0, 255, 255};
  mesh.vertices[61].pos = {0, 0, 0};
  mesh.vertices[61].color = {0, 0, 255, 255};

  for (int i = 1; i < 6; i++) {
    int t = (i - 1) * 12 + 1;

    y = tR + tR * std::cos(i * 30 * PI);
    r = tR * std::sin(i * 30 * PI);
    for (int j = 0; j < 12; j++) {
      x = r * std::sin(j * 30 * PI);
      z = r * std::cos(j * 30 * PI);
      mesh.vertices[t + j].pos = {x, y, z};
      mesh.vertices[t + j].color = {0, 0, 255, 255};
    }
  }

  for (int i = 0; i < 12; i++) {
    mesh.faces[i].vertexIndex[0] = 0;
    mesh.faces[i].vertexIndex[1] = i + 1;
    if (i == 11) {
      mesh.faces[i].vertexIndex[2] = 1;
    } else {
      mesh.faces[i].vertexIndex[2] = i + 2;
    }
  }

  for (int i = 1; i < 5; i++) {
    int t = 12 + (i - 1) * 24;
    for (int j = 0; j < 11; j++) {
      mesh.faces[t].vertexIndex[0] = (i - 1) * 12 + j + 1;
      mesh.faces[t].vertexIndex[1] = (i - 1) * 12 + j + 2;
      mesh.faces[t].vertexIndex[2] = i * 12 + j + 1;
      t++;

      mesh.faces[t].vertexIndex[0] = i * 12 + j + 1;
      mesh.faces[t].vertexIndex[1] = i * 12 + j + 2;
      mesh.faces[t].vertexIndex[2] = (i - 1) * 12 + j + 2;
      t++;
    }
    mesh.faces[t].vertexIndex[0] = (i - 1) * 12 + 12;
    mesh.faces[t].vertexIndex[1] = (i - 1) * 12 + 1;
    mesh.faces[t].vertexIndex[2] = i * 12 + 12;
    t++;

    mesh.faces[t].vertexIndex[0] = i * 12 + 12;
    mesh.faces[t].vertexIndex[1] = i * 12 + 1;
    mesh.faces[t].vertexIndex[2] = (i - 1) * 12 + 1;
  }

  for (int i = 108, j = 49; i < 119; i++, j++) {
    mesh.faces[i].vertexIndex[0] = j;
    mesh.faces[i].vertexIndex[1] = j + 1;
    mesh.faces[i].vertexIndex[2] = 61;
  }
  mesh.faces[119].vertexIndex[0] = 60;
  mesh.faces[119].vertexIndex[1] = 49;
  mesh.faces[119].vertexIndex[2] = 61;

  for (int i = 0; i < mesh.nFaces; i++) {
    getNormalVector(mesh, i);
  }
}
#endif

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
