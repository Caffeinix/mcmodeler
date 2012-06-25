#ifndef SPRITE_ENGINE_H
#define SPRITE_ENGINE_H

#include <QPixmap>

#include "block_orientation.h"

class BlockProperties;
class Texture;

class SpriteEngine {
 public:
  SpriteEngine();

  QPixmap createSprite(const Texture& texture, const BlockProperties& properties,
                       const BlockOrientation* orientation = BlockOrientation::noOrientation());
};

#endif // SPRITE_ENGINE_H
