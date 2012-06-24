#include "skybox_renderable.h"

SkyboxRenderable::SkyboxRenderable(const QVector3D& size)
    : RectangularPrismRenderable(size, RectangularPrismRenderable::kTextureScale) {
}

SkyboxRenderable::Geometry SkyboxRenderable::moveToOrigin(const SkyboxRenderable::Geometry& geometry) {
  QVector3D sz = size();
  // Center the skybox on all three axes.
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry[0][i] -= QVector3D(sz.x() / 2.0f, sz.y() / 2.0f, -sz.z() / 2.0f);
    out_geometry[1][i] -= QVector3D(sz.x() / 2.0f, sz.y() / 2.0f, -sz.z() / 2.0f);
  }
  return out_geometry;
}
