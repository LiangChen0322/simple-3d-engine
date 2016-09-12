
#ifndef CAMERA_H
#define CAMERA_H

#include "Vec.h"

namespace g3
{

/*******************************************************************************
 * Describes a camera object.
 ******************************************************************************/
struct Camera {
  Vec3 eye;
  Vec3 target;

  float zoomFactor;
  int   ratio;
}; // class Camera

struct Light {
  Vec3 pos;
  Vec3 target;
  int angle;
};

} // namespace g3

#endif // CAMERA_H
