#ifndef DOOR_RENDERABLE_H
#define DOOR_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class DoorRenderable : public RectangularPrismRenderable {
 public:
  DoorRenderable();

 protected:
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);

};

#endif // DOOR_RENDERABLE_H
