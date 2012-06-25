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

#include "door_renderable.h"

#include <QDebug>

#include "block_orientation.h"
#include "enums.h"

DoorRenderable::DoorRenderable() : RectangularPrismRenderable(QVector3D(1.0f, 1.0f, 0.1875f), kTextureClip) {
}

DoorRenderable::TextureCoords DoorRenderable::createTextureCoords(const Geometry& geometry) {
  DoorRenderable::TextureCoords coords = RectangularPrismRenderable::createTextureCoords(geometry);
  // Move the texture coordinates for the top and the left and right side so we use the last few pixels rather than the
  // first few.
  QVector<QVector2D> right_texture = coords[kRightFace];
  QVector<QVector2D> left_texture = coords[kLeftFace];
  QVector<QVector2D> top_texture = coords[kTopFace];

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

  coords[kRightFace] = right_texture;
  coords[kLeftFace] = left_texture;
  coords[kTopFace] = top_texture;
  return coords;
}

DoorRenderable::Geometry DoorRenderable::moveToOrigin(const DoorRenderable::Geometry& geometry) {
  QVector3D sz = size();
  // Center the door on the X axis, but keep it flush against the block edge on both other axes.
  Geometry out_geometry = geometry;
  for (int i = 0; i < 4; ++i) {
    out_geometry[0][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -0.5f);
    out_geometry[1][i] -= QVector3D(sz.x() / 2.0f, 0.5f, -0.5f);
  }
  return out_geometry;
}
