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
