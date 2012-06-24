#include "door_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

DoorRenderable::DoorRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 1.0f, 0.1875f), kTextureClip) {
}

DoorRenderable::TextureCoords DoorRenderable::createTextureCoords(
    const Geometry& geometry, TextureSizing sizing) {
  DoorRenderable::TextureCoords coords = RectangularPrismRenderable::createTextureCoords(geometry, sizing);
  // Move the texture coordinates for the top and the left and right side so we use the last few pixels rather than the
  // first few.
  QVector<QVector2D> right_texture = coords[kRightFace];
  QVector<QVector2D> left_texture = coords[kLeftFace];
  QVector<QVector2D> top_texture = coords[kTopFace];

  qDebug() << "Top texture:" << top_texture;

  qreal thickness = right_texture[0].x();
  for (int i = 0; i < right_texture.size(); ++i) {
    right_texture[i] += QVector2D(1 - thickness, 0);
  }
  for (int i = 0; i < left_texture.size(); ++i) {
    left_texture[i] += QVector2D(1 - thickness, 0);
  }
  for (int i = 0; i < top_texture.size(); ++i) {
    top_texture[i] += QVector2D(0, 1 - thickness);
  }

  qDebug() << "Top texture after:" << top_texture;

  coords[kRightFace] = right_texture;
  coords[kLeftFace] = left_texture;
  coords[kTopFace] = top_texture;
  return coords;
}

DoorRenderable::Geometry DoorRenderable::moveToOrigin(const QVector3D& size, const DoorRenderable::Geometry& geometry) {
  // Center the door on the X axis, but keep it flush against the block edge on both other axes.
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry.first[i] -= QVector3D(size.x() / 2.0f, 0.5f, -0.5f);
    out_geometry.second[i] -= QVector3D(size.x() / 2.0f, 0.5f, -0.5f);
  }
  return out_geometry;
}
