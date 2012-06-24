#ifndef BED_RENDERABLE_H
#define BED_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class BedRenderable : public RectangularPrismRenderable {
 public:
  explicit BedRenderable();

 protected:
  virtual TextureCoords createTextureCoords(const Geometry& geometry, TextureSizing sizing);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

};

#endif // BED_RENDERABLE_H
