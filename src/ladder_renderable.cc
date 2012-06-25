#include "ladder_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

LadderRenderable::LadderRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 1.0f, 0.03125f), kTextureClip) {
}

LadderRenderable::Geometry LadderRenderable::moveToOrigin(const LadderRenderable::Geometry& geometry) {
  QVector3D sz = size();
  // Center the ladder on the X axis, but keep it flush against the block edge on both other axes.
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry[0][i] -= QVector3D(sz.x() / 2.0f, 0.5f, 0.5f - sz.z());
    out_geometry[1][i] -= QVector3D(sz.x() / 2.0f, 0.5f, 0.5f - sz.z());
  }
  return out_geometry;
}

Texture LadderRenderable::textureForQuad(int index) const {
  // We only have one texture, so always use it.
  return texture(0);
}

bool LadderRenderable::shouldRenderQuad(int index, const QVector3D &location,
                                        const BlockOrientation *orientation) const {
  // Ladders only have one face, the front one.
  return index == kFrontFace;
}
