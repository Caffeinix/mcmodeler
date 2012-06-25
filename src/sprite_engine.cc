#include "sprite_engine.h"

#include "block_properties.h"
#include "texture.h"

SpriteEngine::SpriteEngine() {
}

QPixmap SpriteEngine::createSprite(const Texture& texture,
                                   const BlockProperties& properties,
                                   const BlockOrientation* orientation) {
  CacheKey key = qMakePair(&properties, orientation);
  QPixmap cached_pixmap = pixmap_cache_.value(key);
  if (!cached_pixmap.isNull()) {
    return cached_pixmap;
  }

  QPixmap pixmap = texture.texturePixmap();
  QPainter painter(&pixmap);
  painter.save();
  switch (properties.geometry()) {
    case kBlockGeometrySlab:
      painter.setCompositionMode(QPainter::CompositionMode_Clear);
      painter.fillRect(0, 0, pixmap.width(), pixmap.height() / 2, Qt::black);
      break;
    case kBlockGeometryStairs:
      painter.setCompositionMode(QPainter::CompositionMode_Clear);
      painter.fillRect(0, 0, pixmap.width() / 2, pixmap.height() / 2, Qt::black);
      break;
    default:
      break;
  }
  painter.restore();

  if (orientation != BlockOrientation::noOrientation() && properties.validOrientations().count() > 1) {
    painter.setPen(QPen(QColor(0, 255, 0, 128), 2.0));
    if (orientation == BlockOrientation::get("Facing south")) {
      painter.drawLine(0, pixmap.height() - 1, pixmap.width(), pixmap.height() - 1);
    } else if (orientation == BlockOrientation::get("Facing east")) {
      painter.drawLine(pixmap.width() - 1, 0, pixmap.width() - 1, pixmap.height());
    } else if (orientation == BlockOrientation::get("Facing north")) {
      painter.drawLine(0, 1, pixmap.width(), 1);
    } else if (orientation == BlockOrientation::get("Facing west")) {
      painter.drawLine(1, 0, 1, pixmap.height());
    }
  }

  pixmap_cache_.insert(key, pixmap);
  return pixmap;
}
