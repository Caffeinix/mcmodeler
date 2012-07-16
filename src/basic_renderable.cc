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

#include "basic_renderable.h"

#include "enums.h"
#include "render_delegate.h"

BasicRenderable::BasicRenderable(const QVector3D& size)
    : size_(size) {
}

BasicRenderable::~BasicRenderable() {}

void BasicRenderable::initialize() {
  Renderable::initialize();
  Geometry geometry = createGeometry();
  TextureCoords texture_coords = createTextureCoords(geometry);
  geometry = moveToOrigin(geometry);
  addGeometry(geometry, texture_coords);
}

void BasicRenderable::appendVertex(const QVector3D& vertex,
                                   const QVector3D& normal,
                                   const QVector2D& tex_coord) {
  vertices_.append(vertex);
  normals_.append(normal);
  tex_coords_.append(tex_coord);
}

void BasicRenderable::addQuad(const QVector3D& a, const QVector3D& b,
                              const QVector3D& c, const QVector3D& d,
                              const QVector<QVector2D> &tex) {
  QVector3D norm = QVector3D::normal(a, b, c);
  appendVertex(a, norm, tex[kBottomLeftCorner]);
  appendVertex(b, norm, tex[kBottomRightCorner]);
  appendVertex(c, norm, tex[kTopRightCorner]);
  appendVertex(d, norm, tex[kTopLeftCorner]);
}

void BasicRenderable::applyOrientationTransform(const BlockOrientation* orientation) const {}

bool BasicRenderable::shouldRenderQuad(int index,
                                       const QVector3D& location,
                                       const BlockOrientation* orientation) const {
  return true;
}

Texture BasicRenderable::textureForQuad(int index, const BlockOrientation* orientation) const {
  return texture(index);
}

int BasicRenderable::textureMinFilter(const BlockOrientation* orientation) const {
  return GL_LINEAR;
}

void BasicRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
  if (!isInitialized()) {
    qWarning() << "Tried to render a BasicRenderable without first calling initialize().";
    return;
  }
  glPushMatrix();
  glTranslatef(location.x(), location.y(), location.z());

  applyOrientationTransform(orientation);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices().constData());
  glNormalPointer(GL_FLOAT, 0, normals().constData());
  glTexCoordPointer(2, GL_FLOAT, 0, textureCoords().constData());
  for (int start = 0; start < vertices().size(); start += 4) {
    if (!shouldRenderQuad(start / 4, location, orientation)) {
      continue;
    }
    glBindTexture(GL_TEXTURE_2D, textureForQuad(start / 4, orientation).textureId());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter(orientation));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLushort indices[4] = { start, start + 1, start + 2, start + 3 };
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, indices);
  }
  glPopMatrix();
}
