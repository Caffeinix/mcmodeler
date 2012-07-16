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

#ifndef BASIC_RENDERABLE_H
#define BASIC_RENDERABLE_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "renderable.h"

/**
  * An abstract subclass of Renderable that provides some basic functionality and a template for initialization.
  * BasicRenderable divides the initialization process into four steps:
  *
  * 1. A vector of polygons in 3D space is created in the createGeometry() method.  The polygons can have an arbitrary
  *    number of vertices and can be represented in any manner desired.
  * 2. The geometry is passed to createTextureCoords(), which returns a set of texture coordinates for each polygon.
  * 3. The geometry is translated so that it is centered on 0, 0, 0.  This is done by moveToOrigin(), and allows the
  *    renderer to place it in the correct spot when it is rendered later.
  * 4. The geometry is converted into a set of quads using addQuad.
  *
  * Once this setup has finished, the geometry can be drawn by BasicRenderable's generic renderAt() implementation.
  * To customize the rendering, you can reimplement applyOrientationTransform() (to apply OpenGL matrix transformations
  * for orientation support), shouldRenderQuad() (to perform face culling, for example) and/or textureForQuad().  All
  * three of these methods have default implementations, so you do not need to override them unless you want to.
  *
  * You can also reimplement renderAt yourself and use the vertices(), normals(), and textureCoords() accessors to get
  * direct access to the raw geometry.
  */
class BasicRenderable : public Renderable {
 public:
  /**
    * Constructs a BasicRenderable with the given size.  Subclasses may obtain the size passed here by calling the
    * protected size() method.
    */
  explicit BasicRenderable(const QVector3D& size);

  /**
    * Virtual destructor.
    */
  virtual ~BasicRenderable();

  /**
    * @copydoc Renderable::initialize()
    * BasicRenderable implements initialize in terms of the four pure virtual methods below.  Subclasses should not
    * need to override it further.
    */
  virtual void initialize();

  /**
    * @copydoc Renderable::renderAt(const QVector3D&, const BlockOrientation*) const
    * BasicRenderable implements renderAt in a generic manner and calls various other methods while rendering to
    * obtain information it needs.  Generally, you should not need to override this, but if you do, you can call
    * vertices(), normals(), and textureCoords() to get the raw rendering data.
    */
  virtual void renderAt(const QVector3D& location, const BlockOrientation* orientation) const;


 protected:
  /**
    * Represents the abstract geometry of this renderable.  The geometry will be interpreted in addGeometry().
    */
  typedef QVector< QVector<QVector3D> > Geometry;

  /**
    * Represents the texture coordinates for this renderable's geometry.  The texture coordinates will be interpreted
    * in addGeometry().
    */
  typedef QVector< QVector<QVector2D> > TextureCoords;

  /**
    * Creates and returns the abstract geometry for this renderable.  The geometry does not need to be directly
    * renderable; it merely needs to contain enough information so that addGeometry() can construct quads to render.
    * As a simple example, a cube might consist of two quads parallel to one another in space.  The other four faces
    * of the cube can be inferred from the two existing quads.
    */
  virtual Geometry createGeometry() = 0;

  /**
    * Creates and returns a set of texture coordinates for \p geometry.  The texture coordinates do not need to be
    * directly renderable; they merely need to contain enough information so that addGeometry() can construct quads to
    * render.  As a simple example, a cube might consist of six four-element vectors indicating the texture coordinates
    * to be used for each face of the cube.
    */
  virtual TextureCoords createTextureCoords(const Geometry& geometry) = 0;

  /**
    * Returns a copy of \p geometry that has been translated such that the center of the block containing the geometry
    * is centered on the origin point (0, 0, 0).  This is often more conveniently done after computing texture
    * coordinates, hence the separate method.  If the geometry returned by your createGeometry() method is already in
    * the right place, this method can simply return it unchanged.
    */
  virtual Geometry moveToOrigin(const Geometry& geometry) = 0;

  /**
    * Converts \p geometry and \p texture_coords into quads to draw.  The quads can be added to what will be drawn by
    * calling the addQuad() method.  Since this class assumes static geometry, this method will only be invoked once,
    * and can afford to be somewhat expensive.
    */
  virtual void addGeometry(const Geometry& geometry, const TextureCoords& texture_coords) = 0;

  /**
    * Applies any desired OpenGL coordinate transformations (glRotate(), glTranslate(), etc.) to account for the given
    * orientation.  You do not need to push a matrix before applying these transforms; they will be applied at the
    * correct time and in the correct context automatically.  The default implementation does nothing.
    */
  virtual void applyOrientationTransform(const BlockOrientation* orientation) const;

  /**
    * Returns true if the quad at \p index should be rendered at \p location with \p orientation.  Subclasses can check
    * with the render delegate to obtain adjacency information and return true or false as necessary.  The default
    * implementation always returns true.
    */
  virtual bool shouldRenderQuad(int index, const QVector3D& location, const BlockOrientation* orientation) const;

  /**
    * Returns the texture that should be used to draw the quad at \p index for a block in \p orientation.  By default,
    * this just calls Renderable::texture() with index, but you may wish to override it if you wish to use the same
    * texture for multiple quads without having to specify it multiple times, or to use a different texture depending
    * on the orientation of the block.
    */
  virtual Texture textureForQuad(int index, const BlockOrientation* orientation) const;

  /**
    * Returns the OpenGL constant describing the filter that should be used to scale down textures.  By default, this
    * returns \c GL_LINEAR, but some blocks may wish to use other filters.
    */
  virtual int textureMinFilter(const BlockOrientation* orientation) const;

  /**
    * Returns the size that was passed into BasicRenderable's constructor.
    */
  const QVector3D& size() const {
    return size_;
  }

  /**
    * Appends \p vertex with normal vector \p normal and texture coordinates \p tex_coord to the vertex list.
    * You should usually not call this directly; call addQuad() instead.
    */
  void appendVertex(const QVector3D& vertex,
                    const QVector3D& normal,
                    const QVector2D& tex_coord);

  /**
    * Adds a quad with corners \p a, \p b, \p c, and \p d to the vertex list.  Normals will be computed automatically
    * (so make sure to specify your corners in the correct order).  Texture coordinates will be computed based on
    * \p tex, which should contain one texture coordinate per corner.
    */
  void addQuad(const QVector3D& a, const QVector3D& b,
               const QVector3D& c, const QVector3D& d,
               const QVector<QVector2D> &tex);

  /**
    * Returns the vertices that have been added to the vertex list.  You only need to call this if you're reimplementing
    * renderAt().
    */
  const QVector<QVector3D>& vertices() const {
    return vertices_;
  }

  /**
    * Returns the normal vectors that have been added to the vertex list.  You only need to call this if you're
    * reimplementing renderAt().
    */
  const QVector<QVector3D>& normals() const {
    return normals_;
  }

  /**
    * Returns the texture coordinates that have been added to the vertex list.  You only need to call this if you're
    * reimplementing renderAt().
    */
  const QVector<QVector2D>& textureCoords() const {
    return tex_coords_;
  }

 private:
  QVector3D size_;
  QVector<QVector3D> vertices_;
  QVector<QVector3D> normals_;
  QVector<QVector2D> tex_coords_;
};

#endif // BASIC_RENDERABLE_H
