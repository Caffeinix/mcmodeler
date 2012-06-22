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

#ifndef BLOCK_PROPERTIES_H
#define BLOCK_PROPERTIES_H

#include <QPoint>
#include <QString>
#include <QVariantMap>
#include <QVector>

#include "block_orientation.h"
#include "enums.h"

/**
  * Provides access to the properties of a particular block type.  Most of this information is only of interest to
  * Renderable subclasses, and is not exposed broadly.  The rest can be acquired by consulting the BlockPrototype.
  * BlockProperties is an immutable class with no non-const methods.
  */
class BlockProperties {
 public:
  /**
    * Constructs an invalid BlockProperties instance.  Because BlockProperties is immutable, this is only useful as a
    * placeholder (for example, in calls to QMap::value).
    */
  BlockProperties();

  /**
    * Constructs a BlockProperties instance out of the given \p block_data map.  This map should contain keys which
    * match the names of \c Q_PROPERTIES of this class.  The \c spriteOffset property will be populated from a
    * two-element list of integers, the \c tileOffsets property will be populated from a list of two-element lists
    * of integers, and the \c validOrientations property will be populated from a list of strings.
    *
    * @note Generally, the only place this constructor needs to be called is inside
    * BlockPrototype::setupBlockProperties().  To get properties like name, transparency, and orientations for an
    * existing block type, go through BlockPrototype instead.
    */
  explicit BlockProperties(const QVariantMap& block_data);

  /**
    * Constructs a BlockProperties instance out of the given data.  Generally, the only place this constructor needs
    * to be called is inside BlockPrototype::setupBlockProperties().  To get properties like name, transparency, and
    * orientations for an existing block type, go through BlockPrototype instead.
    *
    * For parameter documentation, see the getter methods elsewhere in this class.
    */
  BlockProperties(QString name,
                  BlockGeometry geometry,
                  QVector<BlockOrientation*> valid_orientations,
                  QVector<QPoint> tile_offsets,
                  QPoint sprite_offset,
                  bool is_transparent,
                  bool is_biome_grass,
                  bool is_biome_tree);

  BlockProperties(const BlockProperties& other)
      : name_(other.name()),
        geometry_(other.geometry()),
        valid_orientations_(other.validOrientations()),
        tile_offsets_(other.tileOffsets()),
        sprite_offset_(other.spriteOffset()),
        is_transparent_(other.isTransparent()),
        is_biome_grass_(other.isBiomeGrass()),
        is_biome_tree_(other.isBiomeTree()),
        is_valid_(true) {
  }

  BlockProperties& operator=(const BlockProperties& other) {
    name_ = other.name();
    geometry_ = other.geometry();
    valid_orientations_ = other.validOrientations();
    tile_offsets_ = other.tileOffsets();
    sprite_offset_ = other.spriteOffset();
    is_transparent_ = other.isTransparent();
    is_biome_grass_ = other.isBiomeGrass();
    is_biome_tree_ = other.isBiomeTree();
    is_valid_ = true;
    return *this;
  }

  virtual ~BlockProperties() {}

  /**
    * Returns the name of this block type.
    */
  QString name() const;

  /**
    * Returns what kind of geometry this block type uses.
    */
  BlockGeometry geometry() const;

  /**
    * Returns a list of valid orientations for this block type.  The orientations should be ordered such that the first
    * valid orientation is a reasonable default.
    */
  QVector<BlockOrientation*> validOrientations() const;

  /**
    * Returns a list of tile offsets within the terrain.png file.  The number and order of offsets in the list depends
    * on the geometry.  For a cube, there will be six offsets (one per face).  The Renderable subclass is responsible
    * for knowing how to interpret this information.
    */
  QVector<QPoint> tileOffsets() const;

  /**
    * Returns the offset within the terrain.png file of the tile we should use as the sprite for this block type in a
    * 2D context.
    * @todo We should create a TextureLocation type that contains either an offset within terrain.png or a separate
    * resource where we can find the texture, because not all blocks' sprites are in terrain.png.
    */
  QPoint spriteOffset() const;

  /**
    * Returns whether this block is "transparent" in the Minecraft sense.  Transparent blocks may or may not be
    * visibly translucent, but they permit the passage of light, do not hide blocks behind them, and will not suffocate
    * players.  For the purposes of MCModeler, most of these properties are ignored, but adjacent block culling is not
    * performed on transparent blocks.
    */
  bool isTransparent() const;

  /**
    * Returns whether this block should be shaded as a biome-compliant grass texture.  If true, the BlockPrototype will
    * automatically tint the appropriate textures with a grass-green color.
    */
  bool isBiomeGrass() const;

  /**
    * Returns whether this block should be shaded as a biome-compliant tree leaf texture.  If true, the BlockPrototype
    * will automatically tint the appropriate textures with a leafy green color.
    */
  bool isBiomeTree() const;

  /**
    * Returns whether this BlockProperties object is valid.  This will be false if it was created with the default
    * constructor and true otherwise.
    */
  bool isValid() const;

private:
  QString name_;
  BlockGeometry geometry_;
  QVector<BlockOrientation*> valid_orientations_;
  QVector<QPoint> tile_offsets_;
  QPoint sprite_offset_;
  bool is_transparent_;
  bool is_biome_grass_;
  bool is_biome_tree_;
  bool is_valid_;
};

#endif // BLOCK_PROPERTIES_H
