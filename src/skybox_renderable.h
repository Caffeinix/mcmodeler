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

#ifndef SKYBOX_RENDERABLE_H
#define SKYBOX_RENDERABLE_H

#include "rectangular_prism_renderable.h"

/**
  * A RectangularPrismRenderable used for rendering the skybox.  Differs from a generic RectangularPrismRenderable only
  * in that it does not try to sit flat on block boundaries along the _y_ axis.
  */
class SkyboxRenderable : public RectangularPrismRenderable {
 public:
  explicit SkyboxRenderable(const QVector3D& size);

 protected:
  virtual Geometry moveToOrigin(const Geometry& geometry);
};

#endif // SKYBOX_RENDERABLE_H
