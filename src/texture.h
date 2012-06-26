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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>

#include <QMap>
#include <QPainter>
#include <QPair>
#include <QPixmap>
#include <QString>

class QGLWidget;

/**
  * Represents a 2D texture that can be drawn onto 3D geometry.  There are two kinds of textures: those that are loaded
  * directly from a single dedicated image file, and those that are loaded from a particular tile of a sprite sheet
  * image.
  */
class Texture {
 public:
  /**
    * Constructs an empty texture that renders nothing.
    */
  Texture();

  /**
    * Constructs a texture that draws the entire image at \p path.
    * @param widget The OpenGL widget into which this texture can draw.
    * @param path A filesystem or QResource path pointing to an image in a supported format.
    */
  Texture(QGLWidget* widget, const QString& path);

  /**
    * Constructs a texture that draws a tile from a sprite sheet image \p tilesheet, optionally tinting it with \p color
    * using a particular composition mode.
    * @param tilesheet The sprite sheet to use for this texture pixmap.
    * @param path A filesystem or QResource path pointing to an image in a supported format.
    * @param x_index A zero-based index indicating which column the tile we should draw is in.
    * @param y_index A zero-based index indicating which row the tile we should draw is in.
    * @param x_size The width of a single tile.  This must be uniform, and there is no support for margins.
    * @param y_size The height of a single tile.  This must be uniform, and there is no support for margins.
    * @param color The color we should tint the texture before we render it.
    * @param mode The composition mode we should use to fill the texture.  QPainter::CompositionMode_Multiply is often
    *    a good choice here.
    * @note Special care is taken to maintain the alpha channel of the original pixmap when tinting, so you can safely
    *       use composition modes that would normally nuke the alpha channel (such as multiply).
    */
  Texture(QGLWidget* widget, const QPixmap& tilesheet, int x_index, int y_index, int x_size, int y_size,
          QColor color = QColor(Qt::transparent),
          QPainter::CompositionMode mode = QPainter::CompositionMode_Destination);

  /**
    * Constructs a texture that draws a tile from a sprite sheet image at \p path, optionally tinting it with \p color
    * using a particular composition \p mode.
    * @param widget The OpenGL widget into which this texture can draw.
    * @param path A filesystem or QResource path pointing to an image in a supported format.
    * @param x_index A zero-based index indicating which column the tile we should draw is in.
    * @param y_index A zero-based index indicating which row the tile we should draw is in.
    * @param x_size The width of a single tile.  This must be uniform, and there is no support for margins.
    * @param y_size The height of a single tile.  This must be uniform, and there is no support for margins.
    * @param color The color we should tint the texture before we render it.
    * @param mode The composition mode we should use to fill the texture.  QPainter::CompositionMode_Multiply is often
    *    a good choice here.
    * @note The pixmap for \p path will be cached, so you can pass the same path multiple times without fear of multiple
    *       disk seeks.
    * @note Special care is taken to maintain the alpha channel of the original pixmap when tinting, so you can safely
    *       use composition modes that would normally nuke the alpha channel (such as multiply).
    */
  Texture(QGLWidget* widget, const QString& path, int x_index, int y_index, int x_size, int y_size,
          QColor color = QColor(Qt::transparent),
          QPainter::CompositionMode mode = QPainter::CompositionMode_Destination);

  /**
    * Returns the OpenGL texture ID for this texture.  This is created as soon as the texture is constructed and will
    * not change as long as it exists.
    */
  GLuint textureId() const;

  /**
    * Returns the pixmap for this texture.
    */
  QPixmap texturePixmap() const;

 private:
  /**
    * Attempts to find a cached pixmap for \p path.  If there is none, loads the pixmap from disk and caches it for
    * next time.
    */
  QPixmap getOrCreatePixmapForPath(const QString& path);

  /**
    * Creates and returns a texture pixmap from a sub-rectangle of \p tilesheet, optionally tinting it with \p color
    * using a particular composition \p mode.
    * @param tilesheet The sprite sheet to use for this texture pixmap.
    * @param x_index A zero-based index indicating which column the tile we should draw is in.
    * @param y_index A zero-based index indicating which row the tile we should draw is in.
    * @param x_size The width of a single tile.  This must be uniform, and there is no support for margins.
    * @param y_size The height of a single tile.  This must be uniform, and there is no support for margins.
    * @param color The color we should tint the texture before we render it.
    * @param mode The composition mode we should use to fill the texture.  QPainter::CompositionMode_Multiply is often
    *    a good choice here.
    * @note texturePixmap() does not cache the resulting pixmap.  That is the responsibility of the caller.
    */
  QPixmap texturePixmap(const QPixmap& tilesheet, int x_index, int y_index, int x_size, int y_size,
                        QColor color = QColor(Qt::transparent),
                        QPainter::CompositionMode mode = QPainter::CompositionMode_Destination);

  void initWithTile(QGLWidget* widget, const QString& path, int x_index, int y_index, int x_size, int y_size,
                    QColor color, QPainter::CompositionMode mode);
  void initWithTile(QGLWidget* widget, const QPixmap& tilesheet, int x_index, int y_index, int x_size, int y_size,
                    QColor color, QPainter::CompositionMode mode);

  GLuint maybeBindTexture(QGLWidget* widget, const QPixmap& pixmap);

  /** @todo Perhaps use QPixmapCache here instead? */
  static QMap< QString, QPair<QPixmap, GLuint> >* pixmapCache();
  static QMap< QString, QPair<QPixmap, GLuint> >* tileCache();
  GLuint texture_id_;
  QPixmap texture_pixmap_;
};

#endif // TEXTURE_H
