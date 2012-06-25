#ifndef LADDER_RENDERABLE_H
#define LADDER_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class LadderRenderable : public RectangularPrismRenderable {
 public:
  LadderRenderable();

 protected:
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;
  virtual Texture textureForQuad(int index) const;

};

#endif // LADDER_RENDERABLE_H
