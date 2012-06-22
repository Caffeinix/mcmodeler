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

#ifndef BLOCK_INSTANCE_H
#define BLOCK_INSTANCE_H

#include "block_orientation.h"
#include "block_position.h"
#include "block_prototype.h"

/**
  * Represents a particular instance of a block.  Because there can be many hundreds of thousands of these in any given
  * model, this class is very small and contains only the data that can vary from instance to instance of a particular
  * block type (namely its position and orientation).  Everything else is stored in the BlockPrototype accessible
  * through the prototype() method.  This makes BlockInstances small and cheap to create and copy.
  */
class BlockInstance {
 public:
  /**
    * Constructs a BlockInstance.
    *
    * @param prototype The BlockPrototype describing what kind of block this is.
    * @param position The position of this block in the world.
    * @param orientation The orientation of this block.  Valid orientations depend on the prototype, and can be
    *     found by calling BlockPrototype::orientations() and BlockPrototype::defaultOrientation().
    */
  BlockInstance(BlockPrototype* prototype, const BlockPosition& position, BlockOrientation* orientation);
  ~BlockInstance();

  BlockInstance(const BlockInstance& other);
  BlockInstance& operator=(const BlockInstance& other);

  inline BlockPrototype* prototype() const { return prototype_; }
  inline BlockPosition position() const { return position_; }
  inline BlockOrientation* orientation() const { return orientation_; }

  /**
    * Renders this BlockInstance in a 3D context.  Equivalent to prototype()->renderInstance(*this).  The particular
    * context is determined by the BlockPrototype.  This should only be called from within a QGLWidget::paintGL()
    * implementation.
    */
  inline void render() const {
    prototype()->renderInstance(*this);
  }

 private:
  BlockPrototype* prototype_;
  BlockPosition position_;
  BlockOrientation* orientation_;
};

#endif // BLOCK_INSTANCE_H
