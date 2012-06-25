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

#ifndef STAIRS_RENDERABLE_H
#define STAIRS_RENDERABLE_H

#include "enums.h"
#include "basic_renderable.h"

#include <QPair>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

/**
  * A BasicRenderable for stairs.  This renderable renders the stairs in two pieces, a front step and a back step.
  * It does not attempt to do any special face culling, though that would not be a hard change to make.
  */
class StairsRenderable : public BasicRenderable {
 public:

  /**
    * Creates a StairsRenderable with the given \p size.  The stairs block will have two steps, with the front step
    * half the height of the block.
    */
  explicit StairsRenderable(const QVector3D& size);
  virtual ~StairsRenderable () {}

 protected:
  virtual Geometry createGeometry();
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

  virtual void applyOrientationTransform(const BlockOrientation* orientation) const;
  virtual Texture textureForQuad(int index) const;

  virtual TextureCoords createTextureCoordsForBlock(QVector<QVector3D> front, QVector<QVector3D> back);
};

#endif // RECTANGULAR_PRISM_RENDERABLE_H
