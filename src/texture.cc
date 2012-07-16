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

#include "texture.h"

#include <QDebug>
#include <QGLContext>
#include <QPainter>

Texture::Texture() : texture_id_(0) {
}

Texture::Texture(QGLWidget* widget, const QString& path) {
  if (widget) {
    widget->makeCurrent();
  }
  QMap< QString, QPair<QPixmap, GLuint> >* cache = pixmapCache();
  QPair<QPixmap, GLuint> pair = cache->value(path);
  if (!pair.first.isNull()) {
    texture_pixmap_ = pair.first;
  } else {
    texture_pixmap_ = QPixmap(path);
  }

  if (pair.second != 0) {
    texture_id_ = pair.second;
  } else {
    texture_id_ = maybeBindTexture(widget, texture_pixmap_);
    cache->insert(path, qMakePair<QPixmap, GLuint>(texture_pixmap_, texture_id_));
  }
}

GLuint Texture::maybeBindTexture(QGLWidget* widget, const QPixmap& pixmap) {
  if (widget) {
    return widget->bindTexture(pixmap);
  } else {
    return 0;
  }
}

Texture::Texture(QGLWidget* widget, const QString& path, int x_index, int y_index, int x_size, int y_size,
                 QColor color, QPainter::CompositionMode mode) {
  initWithTile(widget, path, x_index, y_index, x_size, y_size, color, mode);
}

Texture::Texture(QGLWidget* widget, const QPixmap& tilesheet, int x_index, int y_index, int x_size, int y_size,
                 QColor color, QPainter::CompositionMode mode) {
  initWithTile(widget, tilesheet, x_index, y_index, x_size, y_size, color, mode);
}

QPixmap Texture::getOrCreatePixmapForPath(const QString& path) {
  QPixmap pixmap;
  QMap< QString, QPair<QPixmap, GLuint> >* pixmap_cache = pixmapCache();
  QPair<QPixmap, GLuint> pair = pixmap_cache->value(path);
  if (!pair.first.isNull()) {
    pixmap = pair.first;
  } else {
    pixmap = QPixmap(path);
    pixmap_cache->insert(path, qMakePair<QPixmap, GLuint>(pixmap, 0));
  }
  return pixmap;
}

QPixmap Texture::texturePixmap(const QPixmap& tilesheet, int x_index, int y_index, int x_size, int y_size,
                               QColor color, QPainter::CompositionMode mode) {
  QPixmap texture_pixmap(x_size, y_size);
  texture_pixmap.fill(Qt::transparent);
  QPainter painter(&texture_pixmap);
  painter.drawPixmap(QRect(0, 0, x_size, y_size), tilesheet,
                     QRect(x_index * x_size, y_index * y_size, x_size, y_size));
  if (color.alpha() > 0 && mode != QPainter::CompositionMode_Destination) {
    painter.setCompositionMode(mode);
    painter.fillRect(0, 0, x_size, y_size, color);

    // Multiply will mess up the alpha channel, so re-apply it from the original master bitmap.
    painter.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
    painter.drawPixmap(QRect(0, 0, x_size, y_size), tilesheet,
                       QRect(x_index * x_size, y_index * y_size, x_size, y_size));
  }
  return texture_pixmap;
}

void Texture::initWithTile(QGLWidget* widget, const QPixmap& tilesheet, int x_index, int y_index, int x_size,
                           int y_size, QColor color, QPainter::CompositionMode mode) {
  if (widget) {
    widget->makeCurrent();
  }
  QMap< QString, QPair<QPixmap, GLuint> >* tile_cache = tileCache();
  QString identifier = QString("%1;%2:%3,%4@%5,%6;%7/%8").arg(reinterpret_cast<qptrdiff>(widget))
                       .arg(tilesheet.cacheKey()).arg(x_index).arg(y_index).arg(x_size).arg(y_size)
                       .arg(color.rgba()).arg(mode);
  QPair<QPixmap, GLuint> pair = tile_cache->value(identifier);
  if (!pair.first.isNull()) {
    texture_pixmap_ = pair.first;
    texture_id_ = pair.second;
  } else {
    texture_pixmap_ = texturePixmap(tilesheet, x_index, y_index, x_size, y_size, color, mode);
    texture_id_ = maybeBindTexture(widget, texture_pixmap_);
    tile_cache->insert(identifier, qMakePair<QPixmap, GLuint>(texture_pixmap_, texture_id_));
  }
}

void Texture::initWithTile(QGLWidget* widget, const QString& path, int x_index, int y_index,
                           int x_size, int y_size, QColor color, QPainter::CompositionMode mode) {
  if (widget) {
    widget->makeCurrent();
  }
  QMap< QString, QPair<QPixmap, GLuint> >* tile_cache = tileCache();
  QString identifier = QString("%1;%2:%3,%4@%5,%6;%7/%8").arg(reinterpret_cast<qptrdiff>(widget)).arg(path)
                       .arg(x_index).arg(y_index).arg(x_size).arg(y_size).arg(color.rgba()).arg(mode);
  QPair<QPixmap, GLuint> pair = tile_cache->value(identifier);
  if (!pair.first.isNull()) {
    texture_pixmap_ = pair.first;
    texture_id_ = pair.second;
  } else {
    QPixmap tilesheet = getOrCreatePixmapForPath(path);
    texture_pixmap_ = texturePixmap(tilesheet, x_index, y_index, x_size, y_size, color, mode);
    texture_id_ = maybeBindTexture(widget, texture_pixmap_);
    tile_cache->insert(identifier, qMakePair<QPixmap, GLuint>(texture_pixmap_, texture_id_));
  }
}

GLuint Texture::textureId() const {
  return texture_id_;
}

QPixmap Texture::texturePixmap() const {
  return texture_pixmap_;
}

QMap< QString, QPair<QPixmap, GLuint> >* Texture::pixmapCache() {
  static QMap< QString, QPair<QPixmap, GLuint> > pixmap_cache_;
  return &pixmap_cache_;
}

QMap< QString, QPair<QPixmap, GLuint> >* Texture::tileCache() {
  static QMap< QString, QPair<QPixmap, GLuint> > tile_cache_;
  return &tile_cache_;
}
