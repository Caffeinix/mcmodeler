#ifndef BED_RENDERABLE_H
#define BED_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class BedRenderable : public RectangularPrismRenderable {
 public:
  BedRenderable();

 protected:
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

};

#endif // BED_RENDERABLE_H
