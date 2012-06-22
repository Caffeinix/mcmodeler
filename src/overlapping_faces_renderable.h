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

#ifndef OVERLAPPING_FACES_RENDERABLE_H
#define OVERLAPPING_FACES_RENDERABLE_H

#include "renderable.h"

#include <QVector3D>
#include <QVector2D>

class OverlappingFacesRenderable : public Renderable {
 public:
  explicit OverlappingFacesRenderable(float size, QVector3D overlap);
  virtual ~OverlappingFacesRenderable() {}

  void renderAt(const QVector3D& location, const BlockOrientation* orientation) const;

 protected:
  void appendVertex(const QVector3D &vertex,
                    const QVector3D &normal,
                    const QVector2D &tex_coord);

  void addQuad(const QVector3D &a, const QVector3D &b,
               const QVector3D &c, const QVector3D &d,
               const QVector<QVector2D> &tex);

 private:
  QVector<QVector3D> vertices_;
  QVector<QVector3D> normals_;
  QVector<QVector2D> tex_coords_;
};

#endif // OVERLAPPING_FACES_RENDERABLE_H
