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

class BlockManager;
class QDataStream;

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
  BlockInstance(BlockPrototype* prototype, const BlockPosition& position, const BlockOrientation* orientation);

  /**
    * Constructs a BlockInstance by deserializing it from \p stream using \p block_manager.  If a block can be read
    * from the stream without skipping any bytes, the created BlockInstance will be valid.  Otherwise, the created
    * BlockInstance will be invalid.  Either way, due to the sequential nature of streams, the stream will be advanced
    * past the data that was read.
    */
  BlockInstance(QDataStream* stream, BlockManager* block_manager);

  /**
    * Constructs an invalid BlockInstance.  This is only useful when using BlockInstance in a context where a default
    * constructor is expected, for instance map lookup.  Invalid BlockInstances return NULL from both prototype() and
    * orientation().
    */
  BlockInstance();

  ~BlockInstance();

  /**
    * Copy constructor.  Creates a BlockInstance that is an exact clone of \p other.
    */
  BlockInstance(const BlockInstance& other);

  /**
    * Assignment operator.  Modifies \c this so that it is identical to \p other.
    */
  BlockInstance& operator=(const BlockInstance& other);

  bool serialize(QDataStream* stream) const;

  /**
    * Returns \c true if this instance is valid (that is, it was constructed with valid data rather than with the
    * default constructor).
    */
  inline bool isValid() const { return is_valid_; }

  /**
    * Returns the BlockPrototype for this block.  This will be NULL if the instance is not valid.
    * @sa isValid()
    */
  inline BlockPrototype* prototype() const { return prototype_; }

  /**
    * Returns the 3D position of this block.  This will be the origin if the instance is not valid.
    * @sa isValid()
    */
  inline BlockPosition position() const { return position_; }

  /**
    * Returns the orientation of this block.  This will be NULL if the instance is not valid.
    * @sa isValid()
    */
  inline const BlockOrientation* orientation() const { return orientation_; }

  /**
    * Renders this BlockInstance in a 3D context.  Equivalent to `prototype()->renderInstance(*this)`.  The particular
    * context is determined by the BlockPrototype.  This should only be called from within a QGLWidget::paintGL()
    * implementation.  If the instance is not valid, this does nothing.
    */
  inline void render() const {
    if (Q_LIKELY(prototype())) {
      prototype()->renderInstance(*this);
    }
  }

 private:
  bool is_valid_;
  BlockPrototype* prototype_;
  BlockPosition position_;
  const BlockOrientation* orientation_;
};

#endif // BLOCK_INSTANCE_H
