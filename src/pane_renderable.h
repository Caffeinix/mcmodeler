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

#include "rectangular_prism_renderable.h"

#ifndef PANE_RENDERABLE_H
#define PANE_RENDERABLE_H

/**
  * Subclass of RectangularPrismRenderable used for drawing panes, like glass panes or iron bars.  Panes actually have
  * fairly little resemblance to rectangular prisms, but they can be constructed out of multiple rectangular prisms in
  * various orientations, so we subclass RectangularPrismRenderable mainly to get its texture coordinate mapping code.
  *
  * To support the myriad different possible orientations of panes, we define a grand total of five different prisms:
  *
  *   * A full-width pane running north-south,
  *   * A half-width pane on the north side of the block,
  *   * A half-width pane on the east side of the block,
  *   * A half-width pane on the south side of the block,
  *   * A half-width pane on the west side of the block.
  *
  * This allows us to construct almost every necessary orientation (including all four corner orientations) by merely
  * hiding or showing certain quads when we render.  The sole exception is a full-width east-west pane, which is
  * rendered by simply rotating the north-south pane before we render it.
  *
  * @todo Arguably, composition would be more appropriate than inheritance for this class.
  */
class PaneRenderable : public RectangularPrismRenderable {
 public:
  explicit PaneRenderable(const QVector3D& size);
  virtual ~PaneRenderable();

 protected:
  enum QuadIndex {
    kFullWidthFront,
    kFullWidthBack,
    kFullWidthBottom,
    kFullWidthRight,
    kFullWidthTop,
    kFullWidthLeft,

    kSouthHalfFront,
    kSouthHalfBack,
    kSouthHalfBottom,
    kSouthHalfRight,
    kSouthHalfTop,
    kSouthHalfLeft,

    kNorthHalfFront,
    kNorthHalfBack,
    kNorthHalfBottom,
    kNorthHalfRight,
    kNorthHalfTop,
    kNorthHalfLeft,

    kWestHalfFront,
    kWestHalfBack,
    kWestHalfBottom,
    kWestHalfRight,
    kWestHalfTop,
    kWestHalfLeft,

    kEastHalfFront,
    kEastHalfBack,
    kEastHalfBottom,
    kEastHalfRight,
    kEastHalfTop,
    kEastHalfLeft
  };

  virtual Geometry createGeometry();
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

  virtual void applyOrientationTransform(const BlockOrientation* orientation) const;
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;
  virtual Texture textureForQuad(int index, const BlockOrientation* orientation) const;
};

#endif // PANE_RENDERABLE_H
