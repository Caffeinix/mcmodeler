#ifndef SPRITE_ENGINE_H
#define SPRITE_ENGINE_H

#include <QMap>
#include <QPair>
#include <QPixmap>

#include "block_orientation.h"
#include "block_type.h"

class BlockProperties;
class Texture;

class SpriteEngine {
 public:
  SpriteEngine();

  QPixmap createSprite(const Texture& texture, const BlockProperties& properties,
                       const BlockOrientation* orientation = BlockOrientation::noOrientation());

 private:
  typedef QPair<const BlockProperties*, const BlockOrientation*> CacheKey;
  QMap<CacheKey, QPixmap> pixmap_cache_;
};

#endif // SPRITE_ENGINE_H
