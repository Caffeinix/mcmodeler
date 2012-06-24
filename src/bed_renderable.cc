#include "bed_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

BedRenderable::BedRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 0.5f, 1.0f)) {
}

BedRenderable::TextureCoords BedRenderable::createTextureCoords(const Geometry& geometry) {
  BedRenderable::TextureCoords coords = RectangularPrismRenderable::createTextureCoords(geometry);
  // Flip the front face horizontally.
  QVector<QVector2D> front_texture = coords[kFrontFace];
  qSwap(front_texture[kBottomLeftCorner], front_texture[kBottomRightCorner]);
  qSwap(front_texture[kTopLeftCorner], front_texture[kTopRightCorner]);
  coords[0] = front_texture;
  return coords;
}

void BedRenderable::addGeometry(const BedRenderable::Geometry& geometry,
                                const BedRenderable::TextureCoords& texture_coords) {
  // add the front face
  addQuad(geometry[0][kBottomLeftCorner], geometry[0][kBottomRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kTopLeftCorner], texture_coords.at(0));

  // add the back face
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kBottomLeftCorner],
          geometry[1][kTopLeftCorner], geometry[1][kTopRightCorner], texture_coords.at(1));

  // add the sides

  // Lift the mattress bottom up past the legs.
  QVector3D mattress_lift = QVector3D(0.0f, 0.25f, 0.0f);
  addQuad(geometry[1][kBottomLeftCorner] + mattress_lift, geometry[1][kBottomRightCorner] + mattress_lift,
          geometry[0][kBottomRightCorner] + mattress_lift, geometry[0][kBottomLeftCorner] + mattress_lift,
          texture_coords.at(2));  // Bottom
  addQuad(geometry[1][kBottomRightCorner], geometry[1][kTopRightCorner],
          geometry[0][kTopRightCorner], geometry[0][kBottomRightCorner], texture_coords.at(3));    // Right
  addQuad(geometry[1][kTopRightCorner], geometry[1][kTopLeftCorner],
          geometry[0][kTopLeftCorner], geometry[0][kTopRightCorner], texture_coords.at(4));        // Top
  addQuad(geometry[1][kTopLeftCorner], geometry[1][kBottomLeftCorner],
          geometry[0][kBottomLeftCorner], geometry[0][kTopLeftCorner], texture_coords.at(5));      // Left
}

