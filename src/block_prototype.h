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

#ifndef BLOCK_PROTOTYPE_H
#define BLOCK_PROTOTYPE_H

#include "block_properties.h"
#include "block_type.h"
#include "renderable.h"
#include "render_delegate.h"
#include "texture.h"

class BlockInstance;
class BlockOracle;
class BlockPosition;
class QGLWidget;

/**
  * Provides access to data and behavior common to a particular kind of block.  Each block type (sand, stone pressure
  * plate, etc) has a single BlockPrototype instance associated with it, which can be accessed through the BlockManager
  * or BlockInstance.  The prototype knows how to render its associated block type (see renderInstance) and also knows
  * about the properties of that block type such as its name, transparency, and valid orientations.
  */
class BlockPrototype : public RenderDelegate {
 public:
  /**
    * Loads information about all known Minecraft block types.  This must be called at startup before making any
    * inquiries about BlockPrototype data.  Because it populates a static data structure, it is not thread-safe.
    */
  static void setupBlockProperties();

  /**
    * Returns the name for a block of the given type.
    * @param type The type of block you want the name of.
    */
  static QString nameOfType(blocktype_t type);

  /**
    * Returns the number of unique block types known to MCModeler.
    */
  static int blockCount();

  /**
    * Constructs a BlockPrototype.
    *
    * @warning Because there should only ever be one BlockPrototype per block type per render destination, you should
    * never call this constructor directly.  Instead, call BlockManager::getPrototype.
    *
    * @param type The type of block this is a prototype for.
    * @param oracle The BlockOracle the prototype will use to determine neighboring face information.
    * @param widget The QGLWidget into which blocks of this type will be rendered.
    */
  explicit BlockPrototype(blocktype_t type, BlockOracle* oracle, QGLWidget* widget);

  virtual bool shouldRenderFace(const Renderable* renderable, Face face, const QVector3D& location) const;

  /**
    * Returns the sprite pixmap that should be used to represent this kind of block in a 2D context.
    */
  QPixmap sprite() const;

  /**
    * Returns the name of this block.  For example, "Netherrack" or "Diamond ore".
    */
  QString name() const {
    return properties().name();
  }

  /**
    * Returns the names of the categories to which this block belongs.
    */
  QStringList categories() const {
    return properties().categories();
  }

  /**
    * Returns the blocktype_t constant for blocks of this type.
    */
  inline blocktype_t type() const {
    return type_;
  }

  /**
    * Returns the default orientation for blocks of this type.  If this block doesn't have any orientations (dirt,
    * cobblestone, most other blocks) it will return the value of BlockOrientation::noOrientation().  Otherwise, the
    * return value will depend on the block type.  Most blocks start out facing south.
    */
  BlockOrientation* defaultOrientation() const;

  /**
    * Returns a vector of valid orientations for this block.  The default orientation will be the first element in the
    * vector.
    */
  QVector<BlockOrientation*> orientations() const;

  /**
    * Returns whether this block is "transparent" in the Minecraft sense.  Transparent blocks may or may not be
    * visibly translucent, but they permit the passage of light, do not hide blocks behind them, and will not suffocate
    * players.  For the purposes of MCModeler, most of these properties are ignored, but adjacent block culling is not
    * performed on transparent blocks.
    */
  bool isTransparent() const {
    return properties().isTransparent();
  }

  /**
    * Renders an instance of this block into the render destination with which the prototype was constructed.  The
    * position and orientation for the block are read from the instance. This should only be called from within a
    * QGLWidget::paintGL() implementation.
    *
    * @param instance The BlockInstance to render.
    */
  void renderInstance(const BlockInstance& instance) const;

 private:
  /**
    * The mapping from blocktype_t enum constants to BlockProperties objects.  This must be a pointer to avoid creating
    * a static of non-POD type.  It is allocated and populated in setupBlockProperties().
    */
  static QMap<blocktype_t, BlockProperties>* s_type_mapping;

  /**
    * Returns the prototype of the block that would be adjacent to the \p face Face of a block of this type that is
    * located at \p location.
    * @return The prototype of the adjacent block, or \c NULL if no block is adjacent to that face.
    */
  BlockPrototype* neighboringBlockForFace(Face face, const QVector3D& location) const;

  /**
    * Returns the BlockProperties object for this prototype.  This is private because much of the information is only
    * of use to the Renderable, but some important fields like name and transparency are exposed by BlockPrototype.
    */
  const BlockProperties& properties() const;

  Texture sprite_texture_;
  BlockProperties properties_;
  blocktype_t type_;
  BlockOracle* oracle_;
  QScopedPointer<Renderable> renderable_;
};

#endif // BLOCK_PROTOTYPE_H
