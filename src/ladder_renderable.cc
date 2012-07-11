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

Texture LadderRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  // We only have one texture, so always use it.
  return texture(0);
}

bool LadderRenderable::shouldRenderQuad(int index, const QVector3D &location,
                                        const BlockOrientation *orientation) const {
  // Ladders only have one face, the front one.
  return index == kFrontFace;
}
