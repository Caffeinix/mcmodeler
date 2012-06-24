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

#include "stairs_renderable.h"

#include "block_orientation.h"
#include "render_delegate.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

StairsRenderable::StairsRenderable(const QVector3D& size)
    : size_(size) {
}

void StairsRenderable::initialize() {
  Geometry geometry = createGeometry(size_);
  TextureCoords texture_coords = createTextureCoords(geometry);
  geometry = moveToOrigin(size_, geometry);
  addGeometry(geometry, texture_coords);
}

void StairsRenderable::addGeometry(const StairsRenderable::Geometry& geometry,
                                   const StairsRenderable::TextureCoords& texture_coords) {
  for (int front = 0, back = 1, coords = 0; back <= 3; front += 2, back += 2, coords += 6) {
    // add the front face
    addQuad(geometry[front][kBottomLeftCorner], geometry[front][kBottomRightCorner],
            geometry[front][kTopRightCorner], geometry[front][kTopLeftCorner], texture_coords.at(coords));

    // add the back face
    addQuad(geometry[back][kBottomRightCorner], geometry[back][kBottomLeftCorner],
            geometry[back][kTopLeftCorner], geometry[back][kTopRightCorner], texture_coords.at(coords + 1));

    // add the sides
    addQuad(geometry[back][kBottomLeftCorner], geometry[back][kBottomRightCorner],
            geometry[front][kBottomRightCorner], geometry[front][kBottomLeftCorner], texture_coords.at(coords + 2));  // Bottom
    addQuad(geometry[back][kBottomRightCorner], geometry[back][kTopRightCorner],
            geometry[front][kTopRightCorner], geometry[front][kBottomRightCorner], texture_coords.at(coords + 3));    // Right
    addQuad(geometry[back][kTopRightCorner], geometry[back][kTopLeftCorner],
            geometry[front][kTopLeftCorner], geometry[front][kTopRightCorner], texture_coords.at(coords + 4));        // Top
    addQuad(geometry[back][kTopLeftCorner], geometry[back][kBottomLeftCorner],
            geometry[front][kBottomLeftCorner], geometry[front][kTopLeftCorner], texture_coords.at(coords + 5));      // Left
  }
}

StairsRenderable::Geometry StairsRenderable::moveToOrigin(
    const QVector3D& size, const StairsRenderable::Geometry& geometry) {
  // Now move verts half cube width across so cube is centered on origin.
  // (But keep it flat on the ground below it.)
  Geometry out_geometry = geometry;
  for (int quad = 0; quad < 4; ++quad) {
    for (int i = 0; i < 4; ++i) {
      out_geometry[quad][i] -= QVector3D(size.x() / 2.0f, 0.5f, -size.z() / 2.0f);
    }
  }
  return out_geometry;
}


StairsRenderable::Geometry StairsRenderable::createGeometry(const QVector3D& size) {
  QVector<QVector3D> verts1;
  verts1.resize(4);
  verts1[kBottomRightCorner].setX(size.x());
  verts1[kTopRightCorner].setX(size.x());
  verts1[kTopRightCorner].setY(size.y() / 2);
  verts1[kTopLeftCorner].setY(size.y() / 2);

  // back face - "extrude" verts down
  QVector<QVector3D> back1(verts1);
  for (int i = 0; i < 4; ++i) {
    back1[i].setZ(-size.z() / 2);
  }

  // Repeat for other block
  QVector<QVector3D> verts2;
  verts2.resize(4);
  verts2[kBottomLeftCorner].setZ(-size.z() / 2);
  verts2[kBottomRightCorner].setZ(-size.z() / 2);
  verts2[kTopRightCorner].setZ(-size.z() / 2);
  verts2[kTopLeftCorner].setZ(-size.z() / 2);
  verts2[kBottomRightCorner].setX(size.x());
  verts2[kTopRightCorner].setX(size.x());
  verts2[kTopRightCorner].setY(size.y());
  verts2[kTopLeftCorner].setY(size.y());

  // back face - "extrude" verts down
  QVector<QVector3D> back2(verts2);
  for (int i = 0; i < 4; ++i) {
    back2[i].setZ(-size.z());
  }

  return Geometry() << verts1 << back1 << verts2 << back2;
}

StairsRenderable::TextureCoords StairsRenderable::createTextureCoords(const Geometry& geometry) {
  TextureCoords front_coords = createTextureCoordsForBlock(geometry[0], geometry[1]);
  TextureCoords back_coords = createTextureCoordsForBlock(geometry[2], geometry[3]);

  // See http://goo.gl/OSz2U.
  front_coords[kLeftFace][kBottomLeftCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kBottomRightCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kTopRightCorner] += QVector2D(0.5, 0);
  front_coords[kLeftFace][kTopLeftCorner] += QVector2D(0.5, 0);

  back_coords[kLeftFace][kBottomLeftCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kBottomRightCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kTopRightCorner] += QVector2D(-0.5, 0);
  back_coords[kLeftFace][kTopLeftCorner] += QVector2D(-0.5, 0);

  return TextureCoords() << front_coords << back_coords;
}

StairsRenderable::TextureCoords StairsRenderable::createTextureCoordsForBlock(QVector<QVector3D> front,
                                                                              QVector<QVector3D> back) {
  QVector<QVector2D> top_tex;
  top_tex.resize(4);
  QVector<QVector2D> front_tex;
  front_tex.resize(4);
  QVector<QVector2D> back_tex;
  back_tex.resize(4);
  QVector<QVector2D> right_tex;
  right_tex.resize(4);
  QVector<QVector2D> bottom_tex;
  bottom_tex.resize(4);
  QVector<QVector2D> left_tex;
  left_tex.resize(4);

  top_tex[kBottomLeftCorner] = QVector2D(front[kTopLeftCorner].x(), -back[kTopLeftCorner].z());
  top_tex[kBottomRightCorner] = QVector2D(front[kTopRightCorner].x(), -back[kTopRightCorner].z());
  top_tex[kTopRightCorner] = QVector2D(front[kBottomRightCorner].x(), -front[kBottomRightCorner].z());
  top_tex[kTopLeftCorner] = QVector2D(front[kBottomLeftCorner].x(), -front[kBottomLeftCorner].z());

  front_tex[kBottomLeftCorner] = front[kBottomLeftCorner].toVector2D();
  front_tex[kBottomRightCorner] = front[kBottomRightCorner].toVector2D();
  front_tex[kTopRightCorner] = front[kTopRightCorner].toVector2D();
  front_tex[kTopLeftCorner] = front[kTopLeftCorner].toVector2D();

  back_tex[kBottomLeftCorner] = front[kBottomLeftCorner].toVector2D();
  back_tex[kBottomRightCorner] = front[kBottomRightCorner].toVector2D();
  back_tex[kTopRightCorner] = front[kTopRightCorner].toVector2D();
  back_tex[kTopLeftCorner] = front[kTopLeftCorner].toVector2D();

  right_tex[kBottomLeftCorner] = QVector2D(-back[kBottomRightCorner].z(), front[kBottomRightCorner].y());
  right_tex[kBottomRightCorner] = QVector2D(-back[kTopRightCorner].z(), front[kTopRightCorner].y());
  right_tex[kTopRightCorner] = QVector2D(-front[kTopLeftCorner].z(), front[kTopLeftCorner].y());
  right_tex[kTopLeftCorner] = QVector2D(-front[kBottomLeftCorner].z(), front[kBottomLeftCorner].y());

  bottom_tex[kBottomLeftCorner] = QVector2D(front[kTopRightCorner].x(), -front[kTopRightCorner].z());
  bottom_tex[kBottomRightCorner] = QVector2D(front[kTopLeftCorner].x(), -front[kTopLeftCorner].z());
  bottom_tex[kTopRightCorner] = QVector2D(front[kBottomLeftCorner].x(), -back[kBottomLeftCorner].z());
  bottom_tex[kTopLeftCorner] = QVector2D(front[kBottomRightCorner].x(), -back[kBottomRightCorner].z());

  left_tex[kBottomLeftCorner] = QVector2D(-front[kTopLeftCorner].z(), front[kTopLeftCorner].y());
  left_tex[kBottomRightCorner] = QVector2D(-front[kBottomLeftCorner].z(), front[kBottomLeftCorner].y());
  left_tex[kTopRightCorner] = QVector2D(-back[kBottomRightCorner].z(), front[kBottomRightCorner].y());
  left_tex[kTopLeftCorner] = QVector2D(-back[kTopRightCorner].z(), front[kTopRightCorner].y());

  return TextureCoords() << front_tex << back_tex << bottom_tex << right_tex << top_tex << left_tex;
}

void StairsRenderable::appendVertex(const QVector3D& vertex,
                                    const QVector3D& normal,
                                    const QVector2D& tex_coord) {
  vertices_.append(vertex);
  normals_.append(normal);
  tex_coords_.append(tex_coord);
}

void StairsRenderable::addQuad(const QVector3D &a, const QVector3D &b,
                               const QVector3D &c, const QVector3D &d,
                               const QVector<QVector2D> &tex) {
  QVector3D norm = QVector3D::normal(a, b, c);
  appendVertex(a, norm, tex[kBottomLeftCorner]);
  appendVertex(b, norm, tex[kBottomRightCorner]);
  appendVertex(c, norm, tex[kTopRightCorner]);
  appendVertex(d, norm, tex[kTopLeftCorner]);
}

void StairsRenderable::renderAt(const QVector3D& location, const BlockOrientation* orientation) const {
  glPushMatrix();
  glTranslatef(location.x(), location.y(), location.z());

  if (orientation == BlockOrientation::get("Facing north")) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing east")) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  } else if (orientation == BlockOrientation::get("Facing west")) {
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices_.constData());
  glNormalPointer(GL_FLOAT, 0, normals_.constData());
  glTexCoordPointer(2, GL_FLOAT, 0, tex_coords_.constData());
  for (int start = 0; start < vertices_.size(); start += 4) {
    // Front, back, bottom, right, top, left.
    Face face = mapToDefaultOrientation(static_cast<Face>(start / 4 % 6), orientation);
    if (renderDelegate() && !renderDelegate()->shouldRenderFace(this, face, location)) {
      continue;
    }
    glBindTexture(GL_TEXTURE_2D, texture(start / 4 % 6).textureId());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLushort indices[4] = { start, start + 1, start + 2, start + 3 };
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, indices);
  }
  glPopMatrix();
}

Face StairsRenderable::mapToDefaultOrientation(Face local_face, const BlockOrientation* orientation) const {
  if (orientation == BlockOrientation::get("Facing north")) {
    switch (local_face) {
    case kFrontFace:
      return kBackFace;
    case kBackFace:
      return kFrontFace;
    case kRightFace:
      return kLeftFace;
    case kLeftFace:
      return kRightFace;
    default:
      break;
    }
  } else if (orientation == BlockOrientation::get("Facing west")) {
    switch (local_face) {
    case kFrontFace:
      return kLeftFace;
    case kBackFace:
      return kRightFace;
    case kRightFace:
      return kFrontFace;
    case kLeftFace:
      return kBackFace;
    default:
      break;
    }
  } else if (orientation == BlockOrientation::get("Facing east")) {
    switch (local_face) {
    case kFrontFace:
      return kRightFace;
    case kBackFace:
      return kLeftFace;
    case kRightFace:
      return kBackFace;
    case kLeftFace:
      return kFrontFace;
    default:
      break;
    }
  }
  return local_face;
}
