#ifndef SKYBOX_RENDERABLE_H
#define SKYBOX_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class SkyboxRenderable : public RectangularPrismRenderable {
 public:
  explicit SkyboxRenderable(const QVector3D& size);

 protected:
  virtual Geometry moveToOrigin(const Geometry& geometry);
};

#endif // SKYBOX_RENDERABLE_H
