
#ifndef _BALL_H_
#define _BALL_H_

#include <cstddef>
#include "Vec.h"
#include "Mat.h"
#include "Quaternion.h"
#include <memory>

namespace g3 {

struct ColorRGBA {
  int r, g, b, a;
};

struct Sphere {
  Vec3 centre;
  // ColorRGBA color;
  float R;
  int res;
};

struct BallMesh {
  unsigned int nVertices;
  std::unique_ptr<Vertex[]> vertices;

  unsigned int nFaces;
  std::unique_ptr<Triangle[]> faces;

  float rotationX, rotationY, rotationZ;

  Vec3 loc;
}; // struct TriangleMesh

}

#endif
