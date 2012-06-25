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

#ifndef SPRITE_ENGINE_H
#define SPRITE_ENGINE_H

#include <QMap>
#include <QPair>
#include <QPixmap>

#include "block_orientation.h"
#include "block_type.h"

class BlockProperties;
class Texture;

/**
  * Constructs and returns sprite images for blocks to represent the block in a 2D context.  The sprite images returned
  * by the engine can take into account the block's geometry, orientation, and whether it is being drawn in the
  * BlockPicker or in the LevelWidget.
  */
class SpriteEngine {
 public:
  SpriteEngine();

  /**
    * Creates and returns a sprite pixmap that can be drawn to represent a block in 2D.  \p texture will be used as
    * the basis for the sprite, with modifications made based on \p properties and \p orientation, if given.  To obtain
    * a sprite suitable for the BlockPicker widget, pass BlockOrientation::paletteOrientation() as \p orientation.
    * @note SpriteEngine caches pixmaps when possible, so this method is fairly inexpensive after the first time a
    * particular block is seen in a particular orientation.
    * @note The caching assumes that a particular \p properties parameter implies the same \p texture each time.
    */
  QPixmap createSprite(const Texture& texture, const BlockProperties& properties,
                       const BlockOrientation* orientation = BlockOrientation::noOrientation());

 private:
  typedef QPair<const BlockProperties*, const BlockOrientation*> CacheKey;
  QMap<CacheKey, QPixmap> pixmap_cache_;
};

#endif // SPRITE_ENGINE_H
