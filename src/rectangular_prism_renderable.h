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

#ifndef RECTANGULAR_PRISM_RENDERABLE_H
#define RECTANGULAR_PRISM_RENDERABLE_H

#include "enums.h"
#include "basic_renderable.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>

/**
  * Renderable for rectangular prisms.  This being Minecraft, most blocks in the game are rectangular prisms, so this is
  * a pretty generic way of rendering things.  It can handle blocks with varying heights (slabs) and blocks that do not
  * occupy the entire cube (chests), and can be easily subclassed to deal with other geometries.
  */
class RectangularPrismRenderable : public BasicRenderable {
 public:

  /**
    * Defines how textures should be fitted to the geometry.
    */
  enum TextureSizing {
    /**
      * Indicates that textures should be rendered at full size (16 pixels to the meter) and clipped if the geometry
      * does not span the entire meter along one or more faces.
      */
    kTextureClip,

    /**
      * Indicates that textures should be scaled down so that the entire texture is drawn on each face of the prism if
      * the geometry does not span the entire meter along one or more faces.
      */
    kTextureScale
  };

  enum FaceCulling {
    kCullHiddenFaces,
    kDoNotCullFaces
  };

  /**
    * Creates a RectangularPrismRenderable with the given \p size.  By default, textures will be clipped, but you can
    * pass a TextureSizing explicitly in \p sizing.  If \p culling is omitted or kCullHiddenFaces is passed, then hidden
    * face culling will be performed when possible.  Even transparent blocks will undergo a limited form of hidden face
    * culling if the adjacent block is of the same type.  To disable this, pass kDoNotCullFaces.
    *
    */
  explicit RectangularPrismRenderable(const QVector3D& size, TextureSizing sizing = kTextureClip,
                                      FaceCulling culling = kCullHiddenFaces);
  virtual ~RectangularPrismRenderable() {}

  virtual void applyOrientationTransform(const BlockOrientation* orientation) const;
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;

 protected:
  TextureSizing sizing() const {
    return sizing_;
  }

  virtual Geometry createGeometry();
  virtual TextureCoords createTextureCoords(const Geometry& geometry);
  virtual Geometry moveToOrigin(const Geometry& geometry);
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords);

  /**
    * Translates faces from \p from_orientation into the default orientation.
    * For example, if \p local_face is kFrontFace, and \p from_orientation is "Facing east" (which is a 90 degree
    * counter-clockwise rotation from the default orientation), then the return value will be kRightFace, because when
    * the right face is rotated 90 degrees counterclockwise, it occupies the same position that the front face would
    * occupy in the default orientation.
    * @param local_face The face to translate into the default orientation.
    * @param from_orientation The orientation to translate from.
    * @note This method is used when determining block adjacency information.  The render delegate does not account for
    * orientation when determining adjacency, so we have to map back to the default orientation before we consult it.
    */
  virtual Face mapToDefaultOrientation(Face local_face, const BlockOrientation* from_orientation) const;

  /**
    * @copydoc
    * If back face culling is turned off for this block, we use a nearest-neighbor filter instead of the default linear
    * filter so we don't screw up the alpha channel.
    */
  virtual int textureMinFilter(const BlockOrientation* orientation) const;

 private:
  TextureSizing sizing_;
  FaceCulling culling_;
};

#endif // RECTANGULAR_PRISM_RENDERABLE_H
