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

#ifndef RENDER_DELEGATE_H
#define RENDER_DELEGATE_H

#include <QVector3D>

#include "enums.h"

class Renderable;

/**
  * A delegate interface exposed to Renderables to give them information about the world.
  */
class RenderDelegate {
 public:
  /**
    * Returns whether \p renderable should render a \p face at \p location.  The return value of this method may be
    * affected by, among other things, what blocks are adjacent to the one at \p location and whether that block and/or
    * adjacent blocks are transparent.
    */
  virtual bool shouldRenderFace(const Renderable* renderable, Face face, const QVector3D& location) const = 0;

  /**
    * Returns a vector of valid orientations for this block.  The default orientation will be the first element in the
    * vector.
    */
  virtual QVector<const BlockOrientation*> orientations() const = 0;
};

#endif // RENDER_DELEGATE_H
