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

#ifndef SKYBOX_H
#define SKYBOX_H

#include <QtOpenGL>

#include <QVector>
#include <QVector2D>
#include <QVector3D>

class Skybox {
public:
  enum Face { FRONT, BACK, BOTTOM, RIGHT, TOP, LEFT };
  explicit Skybox(float size);
  void setTexture(Face face, GLuint tex_id);
  void render() const;
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
  QVector<GLuint> textures_;
};
#endif // SKYBOX_H
