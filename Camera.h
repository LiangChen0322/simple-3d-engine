
#ifndef CAMERA_H
#define CAMERA_H

#include "Vec.h"

namespace g3
{

/*******************************************************************************
 * Describes a camera object.
 ******************************************************************************/
struct Camera
{
  Vec3 eye;
  Vec3 target;

  /**
   * Zoom factor
   */
  float zoomFactor;
}; // class Camera

} // namespace g3

#endif // CAMERA_H
