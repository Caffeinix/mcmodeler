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

#ifndef CUBE_H
#define CUBE_H

#include <QtOpenGL>

#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "enums.h"
#include "texture.h"

class Cube {
public:
  explicit Cube(float size);
  Texture texture(Face face);
  void setTexture(Face face, const Texture& texture);
  void renderAt(const QVector3D& location) const;
private:
  void appendVertex(const QVector3D &vertex,
                    const QVector3D &normal,
                    const QVector2D &tex_coord);

  void addQuad(const QVector3D &a, const QVector3D &b,
               const QVector3D &c, const QVector3D &d,
               const QVector<QVector2D> &tex);
  QVector<QVector3D> vertices;
  QVector<QVector3D> normals;
  QVector<QVector2D> texCoords;
  QVector<Texture> textures_;
};
#endif // CUBE_H
