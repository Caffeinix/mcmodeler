/* Copyright 2012 Brian Ellis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
  coords[kFrontFace] = front_texture;

  // Flip the top face horizontally as well.
  QVector<QVector2D> top_texture = coords[kTopFace];
  qSwap(top_texture[kBottomLeftCorner], top_texture[kBottomRightCorner]);
  qSwap(top_texture[kTopLeftCorner], top_texture[kTopRightCorner]);
  coords[kTopFace] = top_texture;

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

