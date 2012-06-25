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

#ifndef BED_RENDERABLE_H
#define BED_RENDERABLE_H

#include "rectangular_prism_renderable.h"

/**
  * Subclass of RectangularPrismRenderable used for drawing beds.  Beds are similar to slabs except that the bottom
  * face is moved up above the "feet" and two of the textures need to be mirrored horizontally for some reason.
  */
class BedRenderable : public RectangularPrismRenderable {
 public:
  BedRenderable();

 protected:
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

};

#endif // BED_RENDERABLE_H
