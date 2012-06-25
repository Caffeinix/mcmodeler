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

#ifndef DOOR_RENDERABLE_H
#define DOOR_RENDERABLE_H

#include "rectangular_prism_renderable.h"

/**
  * Subclass of RectangularPrismRenderable for rendering doors.  Doors differ from regular prisms because their position
  * is clamped to the block edge (as opposed to being centered in the block) and some of their textures need to be
  * clamped to the end rather than to the beginning.
  */
class DoorRenderable : public RectangularPrismRenderable {
 public:
  DoorRenderable();

 protected:
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);

};

#endif // DOOR_RENDERABLE_H
