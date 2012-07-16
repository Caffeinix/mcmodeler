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

#include "sprite_engine.h"

#include "application.h"
#include "block_geometry.h"
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
    case BlockGeometry::kGeometryStairs:
      if (orientation == BlockOrientation::get("Facing south")) {
        painter.fillRect(0, pixmap.height() / 2, pixmap.width(), pixmap.height(), QColor(0, 0, 0, 96));
      } else if (orientation == BlockOrientation::get("Facing north")) {
        painter.fillRect(0, 0, pixmap.width(), pixmap.height() / 2, QColor(0, 0, 0, 96));
      } else if (orientation == BlockOrientation::get("Facing east")) {
        painter.fillRect(pixmap.width() / 2, 0, pixmap.width() / 2, pixmap.height(), QColor(0, 0, 0, 96));
      } else if (orientation == BlockOrientation::get("Facing west")) {
        painter.fillRect(0, 0, pixmap.width() / 2, pixmap.height(), QColor(0, 0, 0, 96));
      } else {  // Palette orientation
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(0, 0, pixmap.width() / 2, pixmap.height() / 2, Qt::black);
      }
      break;
    case BlockGeometry::kGeometrySlab:
      if (orientation == BlockOrientation::paletteOrientation()) {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(0, 0, pixmap.width(), pixmap.height() / 2, Qt::black);
      } else {
        painter.fillRect(0, 0, pixmap.width(), pixmap.height(), QColor(0, 0, 0, 96));
      }
      break;
    case BlockGeometry::kGeometrySnow:
      if (orientation == BlockOrientation::paletteOrientation()) {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(0, 0, pixmap.width(), 3 * pixmap.height() / 4, Qt::black);
      }
      break;
    case BlockGeometry::kGeometryFlow:
    {
      QHash<int, QColor> colors;

      if (Application::instance()->settings()->value("ColorizeFlows", true).toBool()) {
        colors.insert(1, QColor(0x990000));
        colors.insert(2, QColor(0x996600));
        colors.insert(3, QColor(0x999900));
        colors.insert(4, QColor(0x009900));
        colors.insert(5, QColor(0x009999));
        colors.insert(6, QColor(0x000099));
        colors.insert(7, QColor(0x660099));
      }
      int index = properties.validOrientations().indexOf(orientation);
      QString label("F");
      if (index >= 0) {
        label = QString::number(index + 1);
      }

      // Draw text background.
      painter.setBrush(Qt::white);
      painter.setPen(QPen(painter.brush(), 2.0));
      QPainterPath text_path;
      QSize text_size = painter.fontMetrics().size(Qt::TextSingleLine, label);
      QPoint baseline(pixmap.width() / 2 - text_size.width() / 2,
                      pixmap.height() / 2 + painter.fontMetrics().ascent() / 2 - 1);
      QFont font = painter.font();
      font.setBold(true);
      text_path.addText(baseline, font, label);
      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.drawPath(text_path);

      // Draw text foreground over background.
      painter.fillPath(text_path, colors.value(index + 1, QColor(0x333333)));
      break;
    }
    default:
      if (properties.validOrientations().count() > 1) {
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
      break;
  }
  painter.restore();

  pixmap_cache_.insert(key, pixmap);
  return pixmap;
}
