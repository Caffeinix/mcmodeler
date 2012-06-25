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

#ifndef LADDER_RENDERABLE_H
#define LADDER_RENDERABLE_H

#include "rectangular_prism_renderable.h"

class LadderRenderable : public RectangularPrismRenderable {
 public:
  LadderRenderable();

 protected:
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;
  virtual Texture textureForQuad(int index) const;

};

#endif // LADDER_RENDERABLE_H
