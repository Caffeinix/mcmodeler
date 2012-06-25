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

#ifndef OVERLAPPING_FACES_RENDERABLE_H
#define OVERLAPPING_FACES_RENDERABLE_H

#include "rectangular_prism_renderable.h"

#include <QVector3D>
#include <QVector2D>

/**
  * A RectangularPrismRenderable subclass that renders blocks with slightly overlapping faces.  An example of such a
  * block is a cactus, where the outermost 1/16th of each face is a needle which protrudes through the adjoining face.
  */
class OverlappingFacesRenderable : public RectangularPrismRenderable {
 public:

  /**
    * Creates an OverlappingFacesRenderable with a size of \p size where the faces overlap at the edges by \p overlap.
    */
  OverlappingFacesRenderable(const QVector3D& size, const QVector3D& overlap);
  virtual ~OverlappingFacesRenderable();

  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

 protected:
  QVector3D overlap() const {
    return overlap_;
  }

 private:
  QVector3D overlap_;
};

#endif // OVERLAPPING_FACES_RENDERABLE_H
