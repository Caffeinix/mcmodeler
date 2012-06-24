#ifndef DOOR_RENDERABLE_H
#define DOOR_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class DoorRenderable : public RectangularPrismRenderable {
 public:
  explicit DoorRenderable();

 protected:
  virtual Geometry moveToOrigin(const QVector3D& size, const Geometry& geometry);

};

#endif // DOOR_RENDERABLE_H
