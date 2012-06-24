#include "door_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

DoorRenderable::DoorRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 1.0f, 0.1875f), kTextureClip) {
}

DoorRenderable::Geometry DoorRenderable::moveToOrigin(const QVector3D& size, const DoorRenderable::Geometry& geometry) {
  // Now move verts half cube width across so cube is centered on origin.
  // (But keep it flat on the ground below it.)
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry.first[i] -= QVector3D(size.x() / 2.0f, 0.5f, -(0.5f - size.z()));
    out_geometry.second[i] -= QVector3D(size.x() / 2.0f, 0.5f, -(0.5f - size.z()));
  }
  return out_geometry;
}
