#include "bed_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

BedRenderable::BedRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 0.5f, 1.0f)) {
}

BedRenderable::TextureCoords BedRenderable::createTextureCoords(
    const Geometry& geometry, TextureSizing sizing) {
  qDebug() << "In BedRenderable::createTextureCoords()";
  BedRenderable::TextureCoords coords = RectangularPrismRenderable::createTextureCoords(geometry, sizing);
  // Flip the front face horizontally.
  QVector<QVector2D> front_texture = coords[0];
  qSwap(front_texture[0], front_texture[1]);
  qSwap(front_texture[2], front_texture[3]);
  coords[0] = front_texture;
  return coords;
}

void BedRenderable::addGeometry(const BedRenderable::Geometry& geometry,
                                const BedRenderable::TextureCoords& texture_coords) {
  // add the front face
  addQuad(geometry.first[kBottomLeftCorner], geometry.first[kBottomRightCorner],
          geometry.first[kTopRightCorner], geometry.first[kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(geometry.second[kBottomRightCorner], geometry.second[kBottomLeftCorner],
          geometry.second[kTopLeftCorner], geometry.second[kTopRightCorner], texture_coords.at(1));

  // add the sides

  // Lift the mattress bottom up past the legs.
  QVector3D mattress_lift = QVector3D(0.0f, 0.25f, 0.0f);
  addQuad(geometry.second[kBottomLeftCorner] + mattress_lift, geometry.second[kBottomRightCorner] + mattress_lift,
          geometry.first[kBottomRightCorner] + mattress_lift, geometry.first[kBottomLeftCorner] + mattress_lift,
          texture_coords.at(2));  // Bottom
  addQuad(geometry.second[kBottomRightCorner], geometry.second[kTopRightCorner],
          geometry.first[kTopRightCorner], geometry.first[kBottomRightCorner], texture_coords.at(3));    // Right
  addQuad(geometry.second[kTopRightCorner], geometry.second[kTopLeftCorner],
          geometry.first[kTopLeftCorner], geometry.first[kTopRightCorner], texture_coords.at(4));        // Top
  addQuad(geometry.second[kTopLeftCorner], geometry.second[kBottomLeftCorner],
          geometry.first[kBottomLeftCorner], geometry.first[kTopLeftCorner], texture_coords.at(5));      // Left
}

