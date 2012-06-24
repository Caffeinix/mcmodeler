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

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QVector>

#include "texture.h"

class BlockOrientation;
class RenderDelegate;

/**
  * Abstract class representing objects that can be rendered in 3D.  Each BlockPrototype has an associated Renderable,
  * which is instantiated as a particular subclass depending on the block geometry.  The textures for the block type
  * are passed to the Renderable using setTexture().  Each Renderable subclass expects a certain number of textures,
  * and renders them on the geometry it knows how to draw.  The same Renderable object is used to draw every instance
  * of a block type.
  */
class Renderable {
 public:
  Renderable();

  virtual ~Renderable() {}

  /**
    * Initializes this Renderable.
    * This must be called before the renderable can be used to draw geometry.
    */
  virtual void initialize();

  /**
    * Associates \p texture with \p local_id.
    * @param local_id An ID number which will have meaning to the particular Renderable subclass that is doing the
    *     rendering.  For efficiency, keep these numbers small and contiguous, preferably starting at zero.
    * @param texture The texture that should be associated with this ID.
    */
  void setTexture(int local_id, const Texture& texture);

  /**
    * For certain operations (currently determining whether we should skip rendering a particular face due to its
    * adjacency with a solid block), we need to consult a delegate with more information on the block we're rendering
    * than we do.  This is usually the BlockPrototype itself, but Renderable doesn't know that.
    * @todo This API may soon change, since BlockPrototype shouldn't really be dealing with this stuff.
    */
  void setRenderDelegate(RenderDelegate* delegate) {
    delegate_ = delegate;
  }

  /**
    * Renders the textures and geometry this Renderable knows how to draw at the given location and orientation.
    * This method is implemented differently by each Renderable subclass.
    * @warning You must call initialize() before calling this method.
    */
  virtual void renderAt(const QVector3D& location, const BlockOrientation* orientation) const = 0;

 protected:
  /**
    * Returns the texture with the given local ID.
    * @warning If you pass an ID that hasn't been set, we crash, so don't do that.
    */
  Texture texture(int local_id) const;

  /**
    * Returns the render delegate.
    */
  RenderDelegate* renderDelegate() const {
    return delegate_;
  }

 private:
  QVector<Texture> textures_;
  RenderDelegate* delegate_;
};

#endif // RENDERABLE_H
